#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <list>
#include <iomanip>

#include "split.h"

using namespace std;

// Each Customer object contains name, ID, Phone#, email, and an address
class Customer
{
private:
    int cust_id;
    string name, street, city, state, zip, phone, email;

public:
    // Customer Constructor.
    Customer(int id, const string &n, const string &str, const string &c, const string &st, const string &z, const string &p, const string &e) : cust_id(id), name(n), street(str), city(c), state(st), zip(z), phone(p), email(e) {}
    // Returns a string
    string print_detail()
    {
        ostringstream o_stream;
        string out;
        o_stream << "\nCustomer ID #" << cust_id << ":\n"
                 << name << ", ph. " << phone << ", email: " << email << "\n"
                 << street << "\n"
                 << city << ", " << state << " " << zip << endl;
        out = o_stream.str();
        return out;
    }
    int get_id()
    {
        return cust_id;
    }
};
// Each item object contains an item ID, a description of the item, and a price for the item.
class Item
{
private:
    int item_id;
    string description;
    double price;

public:
    // Item constructor.
    Item(int id, const string &desc, double price) : item_id(id), description(desc), price(price) {}
    //  << operator overloader. Outputs ID, Description, and Price of Item.
    friend ostream &operator<<(ostream &out, const Item &item)
    {
        out << "ID: " << item.item_id << " Description: " << item.description;
        return out;
    }
    int get_id()
    {
        return item_id;
    }
    double get_price()
    {
        return price;
    }
};
// Global vector variables
vector<Item> items;
vector<Customer> customers;
// reads from items.txt, creates an item object and adds it to the items vector.
void read_items(const string &filename)
{
    ifstream inFS(filename);
    string line;
    vector<string> vLine;
    getline(inFS, line);
    while (!inFS.eof())
    {
        vLine = split(line, ',');
        items.push_back(Item(stoi(vLine[0]), vLine[1], stod(vLine[2]))); // Item_ID, Description, Price adds the item object to the global items vector
        getline(inFS, line);
    }
    inFS.close();
}
// Returns the Items index associated with the item_id parameter.
int find_item(int item_id)
{
    for (int i = 0; i < items.size(); ++i)
    {
        if (items[i].get_id() == item_id)
        {
            return i;
        }
    }
}
// Returns Customers index associated with given customer_id
int find_customer(int cust_id)
{
    for (int i = 0; i < customers.size(); ++i)
    {
        if (customers[i].get_id() == cust_id)
        {
            return i;
        }
    }
}
// class stores an item_id and the quantity. Can use that info to calcualte the sub total cost of item
class LineItem
{
private:
    int item_id, qty;

public:
    // Constructor, takes in an item ID and the quantity of item.
    LineItem(int id, int qty) : item_id(id), qty(qty) {}
    // < Operator overloader. Helps with sorting the item ID's.
    friend bool operator<(const LineItem &item1, const LineItem &item2)
    {
        return item1.item_id < item2.item_id;
    }
    // Returns the sub total amount of the particular item
    double sub_total()
    {
        int item_in = find_item(item_id);
        return (items[item_in].get_price() * qty);
    }
    int get_id()
    {
        return item_id;
    }
    int get_qty()
    {
        return qty;
    }
};
// Abstract base class for the different types of payment (Credit, Paypal, WireTransfer)
class Payment
{
private:
    double amount;
    friend class Order;

public:
    // Pure virtual function for Credit, PayPal and WireTransfer
    Payment() = default;
    virtual ~Payment() = default;
    virtual string print_details() const = 0; // Pure virtual function. Depends on type of payment.
};
// Subclass of payment, takes card_number and expiration
class Credit : public Payment
{
private:
    string card_number, expiration;

public:
    // Constructor
    Credit(string num, string exp) : card_number(num), expiration(exp) {}
    string print_details() const override
    {
        return "Credit card " + card_number + ", exp. " + expiration;
    }
};

// subclass of payment, takes PayPal ID
class PayPal : public Payment
{
private:
    string paypal_id;

public:
    // Constructor
    PayPal(string id) : paypal_id(id) {}
    string print_details() const override
    {
        string out;
        return ("Paypal ID: " + paypal_id);
    }
};
// Subclass of Payment, contains bank ID, and account ID
class WireTransfer : public Payment
{
private:
    string bank_id, account_id;

public:
    // Constructor
    WireTransfer(string b, string a) : bank_id(b), account_id(a) {}
    string print_details() const override
    {
        return "Wire transfer from Bank ID " + bank_id + ", Account# " + account_id;
    }
};
// Organizes the info to write to the order_report.txt
class Order
{
private:
    int order_id, cust_id;
    string order_date;
    vector<LineItem> line_items;
    Payment *payment;

public:
    Order(int c_id, int o_id, string o_d, vector<LineItem> v, Payment *p) : cust_id(c_id), order_id(o_id), order_date(o_d), line_items(v), payment(p) {}
    ~Order()
    {
        delete payment;
    }
    // returns the total amount of all items purchased.
    double total() const
    {
        double total = 0;
        for (auto item : line_items)
        {
            total += item.sub_total();
        }
        payment->amount = total; // sets the total amount in class Payment.
        return total;
    }
    // returns a string of the order
    string print_order() const
    {
        string out_string;
        ostringstream oSS;
        int customer = find_customer(cust_id); // returns Customer object associated with cust_id.
        oSS << string(27, '=') << endl
            << "Order #" << order_id << ", Date: " << order_date << endl
            << "Amount $" << total() << " Paid by " << payment->print_details() << endl
            << customers[customer].print_detail() << "\nOrder Detail:\n";
        for (auto line_item : line_items)
        {
            int item = find_item(line_item.get_id());
            oSS << "\t" << items[item] << ", " << line_item.get_qty() << " @ " << fixed << setprecision(2) << items[item].get_price() << endl;
        }
        out_string = oSS.str(); // Converts the output stream to a string
        return out_string;
    }
};

// Reads customers.txt file
void read_customers(const string filename)
{
    ifstream inFS(filename);
    string line;
    vector<string> vLine;
    getline(inFS, line);
    while (!inFS.eof())
    {
        vLine = split(line, ',');
        customers.push_back(Customer(stoi(vLine[0]), vLine[1], vLine[2], vLine[3], vLine[4], vLine[5], vLine[6], vLine[7]));
        getline(inFS, line);
    }
    inFS.close();
}
// Global Order list
list<Order> orders;
// Reads from orders.txt creates an Order object and adds it to the orders list.
void read_orders(const string filename)
{
    // 0 = cust_id, 1 = order_id, 2 = date in vLine
    ifstream inFS(filename);
    string line;
    Payment *payment;
    vector<string> vLine, item_qty, line2; // line2 = the payment line or "second line" in orders.txt
    vector<LineItem> line_item;
    while (getline(inFS, line))
    {
        vLine = split(line, ',');
        for (int i = 3; i < vLine.size(); ++i)
        {
            item_qty = split(vLine[i], '-');                                     // vector of size 2. First element is item, second element is quantity.
            line_item.push_back(LineItem(stoi(item_qty[0]), stoi(item_qty[1]))); // Creates a LineItem object and appends to a vector
        }
        sort(line_item.begin(), line_item.end()); // sorts the LineItem objects by item_id's.
        getline(inFS, line);
        line2 = split(line, ',');
        switch (stoi(line2.at(0)))
        {
        case 1: // Paid with credit card
            payment = new Credit(line2[1], line2[2]);
            break;
        case 2: // paid with PayPal
            payment = new PayPal(line2[1]);
            break;
        case 3: // Paid through wire transfer.
            payment = new WireTransfer(line2[1], line2[2]);
            break;
        }
        orders.emplace_back(stoi(vLine[0]), stoi(vLine[1]), vLine[2], line_item, payment); // emblace creates and pushes back.
        line_item.clear();                                                                 // clears the vector for the next order items
    }
}

int main()
{
    double sub_total;
    read_customers("customers.txt");
    read_items("items.txt");
    read_orders("orders.txt");
    ofstream ofs("order_report.txt");
    for (const auto &order : orders)
        ofs << order.print_order() << endl;
    return 0;
}
