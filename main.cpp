#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <fstream>

using namespace std;

// This is your 'Order' - The heart of the LOB
struct Order {
  uint64_t id;
  uint64_t price;
  uint32_t quantity;
  bool is_buy;
};
struct Trade {
  uint64_t buyer_id;
  uint64_t seller_id;
  uint64_t match_price;
  uint32_t quantity;
};


// we need a current bid price and a current ask price to constantly match with different orders
// so get order should have atleast 4 elements 1. get both sides of prices 2. match the elements 3. conditions for when user
// decides if they want to buy or sell and 3. parse it into a orderbook to record the transaction
class LimitOrderBook {
private:
    // storing orders in maps
    // Sell orders aka Asks: Key is price (Ascending), Value is quantity
    map<uint64_t, uint32_t> asks; 
    // Buy orders aka Bids: Key is price (Descending), Value is quantity
    map<uint64_t, uint32_t, greater<uint64_t>> bids; 

public:
    void processOrder(Order od) {

      
        if (od.is_buy) {
            while (od.quantity > 0 && !asks.empty() && od.price >= asks.begin()->first) {
                auto best_ask = asks.begin();
                uint32_t trade_qty = min(od.quantity, best_ask->second);
                //comment out cout when testing (for performance)
                cout << "Order Filled : " << best_ask->first << " Qty: " << trade_qty << endl;

                Trade t = {
                od.id,           // Buyer
                0,               // Seller (0 for seller , 1 for buyer)
                best_ask->first, // Price
                trade_qty        // Qty
                };

                trades_occurred.push_back(t);  

                od.quantity -= trade_qty;
                best_ask->second -= trade_qty;

                if (best_ask->second == 0) asks.erase(best_ask);

                
            }
            
            if (od.quantity > 0) bids[od.price] += od.quantity;
        } 
        else {
             while (od.quantity > 0 && !bids.empty() && od.price <= bids.begin()->first) {
                auto best_bid = bids.begin();
                uint32_t trade_qty = min(od.quantity, best_bid->second);
                //comment out cout when testing (for performance)
                cout << "Order filled : " << best_bid->first << " Qty: " << trade_qty << endl;

                trades_occurred.push_back(t); 

                od.quantity -= trade_qty;
                best_bid->second -= trade_qty;

                if (best_bid->second == 0) bids.erase(best_bid);

                
            }
            if (od.quantity > 0) asks[od.price] += od.quantity;
        }
    }
};


class Timer {};



int main() {

  const int num_orders = 1000000; 

  LimitOrderBook lob;
  
  vector<Order> orders(num_orders);

  ofstream trade_file(trades.csv);
  trade_file << "BuyerId , SellerId , Price , Quantity\n";

  mt19937 gen(42);
  normal_distribution<double> price_dist(100.0, 5.0);
  uniform_int_distribution<uint32_t> qty_dist(1, 100);

  // Start timing exactly before the CPU crunch
  auto start = chrono::high_resolution_clock::now();

  // connect this with LOB class
  for (int i = 0; i < num_orders; ++i) {
    
    uint64_t price_in_cents = static_cast<uint64_t>(price_dist(gen) * 100);

    orders[i] = {
        static_cast<uint64_t>(i), price_dist(gen), qty_dist(gen),
        (i % 2 == 0) // Alternating Buy & Sell
    };

    lob.processOrder(orders[i]);

    vector<Trade> FilledOrders = lob.processOrder(orders[i]);

    for (const auto& t : matches) {
      trade_file << t.buyer_id << "," << t.seller_id << ","  << t.match_price << "," << t.quantity << "\n";
    }
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
