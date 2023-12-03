#include <algorithm>
#include <chrono>
#include <cilk/cilk.h>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>
#include <sys/time.h>
#include <vector>

using namespace std;

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

constexpr int N = 100'000'000;
constexpr int BLOCK_SIZE = 12;
vector<int> arr(N);

size_t partition(size_t begin, size_t end, int pivot) {
  while (begin < end) {
    if (arr[begin] < pivot) {
      begin++;
    } else {
      end--;
      swap(arr[begin], arr[end]);
    }
  }
  return end;
}

void bubble_sort(size_t l, size_t r) {
  bool swapped;
  for (size_t i = l; i < r; i++) {
    swapped = false;
    for (int j = l; j - l < r - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        swap(arr[j], arr[j + 1]);
        swapped = true;
      }
    }

    if (swapped == false)
      break;
  }
}

void seq_qsort(size_t l, size_t r) {
  if (l + BLOCK_SIZE < r) {
    int last = arr[r - 1];
    size_t mid = partition(l, r - 1, last);
    swap(arr[r - 1], arr[mid]);
    seq_qsort(mid + 1, r);
    seq_qsort(l, mid);
  } else {
    bubble_sort(l, r);
  }
}

void par_qsort(size_t l, size_t r) {
  if (l + BLOCK_SIZE < r) {
    int last = arr[r - 1];
    size_t mid = partition(l, r - 1, last);
    swap(arr[r - 1], arr[mid]);
    cilk_scope {
      cilk_spawn par_qsort(mid + 1, r);
      par_qsort(l, mid);
    }
  } else {
    bubble_sort(l, r);
  }
}

size_t test(function<void(size_t l, size_t r)> f) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist6(1, N);

  size_t it_count = 5;

  size_t sum = 0;

  for (int i = 0; i < it_count; i++) {

    for (int j = 0; j < N; j++) {
      arr[j] = dist6(rng);
    }

    size_t start =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count();

    f(0, N);

    sum += duration_cast<milliseconds>(system_clock::now().time_since_epoch())
               .count() -
           start;

    bool pass = std::is_sorted(arr.cbegin(), arr.cend());
    std::cout << "Sort " << ((pass) ? "succeeded" : "failed") << "\n";

    cout << sum << endl;
  }

  return sum;
}

int main() {
  size_t par = test(par_qsort);
  size_t seq = test(seq_qsort);

  cout << "par qsort: " << par << endl;
  cout << "seq qsort: " << seq << endl;
  cout << "acceleration: " << (long double)(seq) / par << "x" << endl;
}
