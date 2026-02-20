#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

// This is your 'Order' - The heart of the LOB
struct Order {
  uint64_t id;
  double price;
  uint32_t quantity;
  bool is_buy;
};

class LimitOrderBook {
private:
};

class Timer {};

int main() {

  const int num_orders = 1000000; // Let's do a MILLION orders

  // ARCHITECTURE: Pre-allocate a 'Slab' of memory
  // Using vector(size) instead of reserve to avoid the crash you had
  vector<Order> orders(num_orders);

  mt19937 gen(42);
  normal_distribution<double> price_dist(100.0, 5.0);
  uniform_int_distribution<uint32_t> qty_dist(1, 100);

  // OPTIMIZATION: Start timing exactly before the CPU crunch
  auto start = chrono::high_resolution_clock::now();

  for (int i = 0; i < num_orders; ++i) {
    orders[i] = {
        static_cast<uint64_t>(i), price_dist(gen), qty_dist(gen),
        (i % 2 == 0) // Alternating Buy/Sell
    };
  }

  auto end = chrono::high_resolution_clock::now();

  // RESULTS
  auto duration =
      chrono::duration_cast<chrono::microseconds>(end - start).count();
  cout << "Generated " << num_orders << " orders in " << duration << " us"
       << endl;
  cout << "Speed: " << (double)duration / num_orders * 1000 << " ns per order"
       << endl;

  return 0;
}
