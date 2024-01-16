#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
Transaction::Transaction( std::string ticker_symbol,  unsigned int day_date,
  unsigned int month_date,  unsigned year_date,
  bool buy_sell_trans,  unsigned int number_shares,
  double trans_amount ){

  day = day_date;
  month = month_date;
  year = year_date;
  symbol = ticker_symbol;
  if (buy_sell_trans) {
    trans_type = "Buy";
    } else {
      trans_type = "Sell";
    }
  shares = number_shares;
  amount = trans_amount;

  p_next = nullptr; //end of list
  trans_id = assigned_trans_id;
  assigned_trans_id++; //increment

}

// Destructor
Transaction::~Transaction(){
  p_next = nullptr;
}

// Overloaded < operator.
bool Transaction::operator<(Transaction const &other) {
  if (year<other.year) {
    return true;
  } else if (year>other.year) {
    return false;
  }
  if (month<other.month) {
    return true;
  } else if (month>other.month) {
    return false;
  }
  if (day<other.day) {
    return true;
  } else if (day>other.day) {
    return false;
  }
  return (trans_id < other.trans_id);

}

// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//


// Constructor
History::History() {
  // Initializes head pointer to nullptr
  p_head = nullptr;
}

// Destructor
History::~History() {
Transaction *current = nullptr;
  // Iterates through  linked list and delete each transaction
  for (Transaction *current = p_head; current != nullptr; ) {
        Transaction *temp = current;
        current = current->get_next();
        delete temp;
        temp = nullptr;
  }
delete current;
current = nullptr;
}

// insert(...): Insert transaction into linked list.
void History::insert(Transaction *p_new_trans) {
  // If the list is empty, sets pointer to the new transaction
  if (p_head == nullptr) {
    p_head = p_new_trans;
    p_head -> set_next(nullptr);
  } else {
    Transaction *counter = p_head;
    while (counter -> get_next() != nullptr) {
      counter = counter -> get_next();
    }
    counter -> set_next(p_new_trans);
    p_new_trans -> set_next(nullptr);
  }

}

// read_history(...): Read the transaction history from file.
void History::read_history() {
  ece150::open_file();

  while (ece150::next_trans_entry()) {
        Transaction *assign = new Transaction(ece150::get_trans_symbol(), ece150::get_trans_day(), ece150::get_trans_month(), ece150::get_trans_year(), ece150::get_trans_type(), ece150::get_trans_shares(), ece150::get_trans_amount()
        );
      //insert values into assign
      insert(assign);
  }
//close after reading
ece150::close_file();
}

// print() Print the transaction history.
void History::print() {
std::cout << "========== BEGIN TRANSACTION HISTORY ============" << std::endl;
    Transaction *counter = p_head;
while (counter != nullptr) {
  counter -> print();
  counter = counter -> get_next();
  }
std::cout << "========== END TRANSACTION HISTORY ============" << std::endl;
}

// sort_by_date(): Sort the linked list by trade date.
void History::sort_by_date() {
  // list to update
  Transaction* sorted_list = nullptr;

  // iterate through the unsorted list until it reaches the end
  while(p_head != nullptr){
    Transaction* cur_node = p_head;
    p_head = p_head->get_next();
    cur_node->set_next(nullptr);

    Transaction* sorted_prev_node = nullptr;
    Transaction* sorted_cur_node = sorted_list;
    while(true){

      if(sorted_cur_node == nullptr && sorted_prev_node == nullptr){
        sorted_list = cur_node;
        break;
      }
      else if(sorted_cur_node == nullptr){
        sorted_prev_node->set_next(cur_node);
        break;
      }

      if(*cur_node < *sorted_cur_node){
        Transaction* temp = sorted_cur_node;
        if(sorted_prev_node == nullptr){
          sorted_list = cur_node;
          cur_node->set_next(temp);
        }
        else{
          sorted_prev_node->set_next(cur_node);
          cur_node->set_next(temp);
        }
        break;
      }

      // iterate through
      sorted_prev_node = sorted_cur_node;
      sorted_cur_node = sorted_cur_node->get_next();

    }
  }
  p_head = sorted_list;
}

// update_acb_cgl(): Updates the ACB and CGL values.
void History::update_acb_cgl() {
  Transaction* cur_node = p_head;
  Transaction* prev_node = nullptr;
  while(cur_node != nullptr){
    // if buy
    if(cur_node->get_trans_type()){
      if(prev_node == nullptr){
        cur_node->set_acb(cur_node->get_amount());
        cur_node->set_share_balance(cur_node->get_shares());
        cur_node->set_acb_per_share(cur_node->get_acb() / cur_node->get_share_balance());
      }
      else{
        cur_node->set_acb(prev_node->get_acb() + cur_node->get_amount());
        cur_node->set_share_balance(prev_node->get_share_balance() + cur_node->get_shares());
        cur_node->set_acb_per_share(cur_node->get_acb() / cur_node->get_share_balance());
      }
    }
    else{ // if sell (assuming there needs to be a buy before a sell, i.e. share bal >= 0)
      cur_node->set_acb_per_share(prev_node->get_acb_per_share()); // copy over previous node's acb per share
      cur_node->set_acb(prev_node->get_acb() - (cur_node->get_shares()*cur_node->get_acb_per_share()));
      cur_node->set_share_balance(prev_node->get_share_balance() - cur_node->get_shares());
      cur_node->set_cgl(cur_node->get_amount() - (cur_node->get_shares()*cur_node->get_acb_per_share()));
    }

    prev_node = cur_node;
    cur_node = cur_node->get_next();
  }
}

// compute_cgl(): )Compute the ACB, and CGL.
double History::compute_cgl(unsigned int year) {
  update_acb_cgl(); // assuming not called before compute_cgl
  double ret = 0;
  Transaction* cur_node = p_head;
  while(cur_node != nullptr){
    if(cur_node->get_year() == year){
      ret += cur_node->get_cgl();
    }
    cur_node = cur_node->get_next();
  }
  return ret;
}


// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { return p_head; }
