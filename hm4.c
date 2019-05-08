/*
* hm4.c
*
*  Started On: Feb 3, 2017
*      Author: Yaro Salo
*/
#define MAX_CUSTOMERS 20
#define MAX_PURCHASES 10
#define MAX_NAME_LEN 29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Holds information about a purchase. */
typedef struct {
    char item_n[MAX_NAME_LEN + 1]; // name
    int item_q;                    // quantity
    float item_p;                  // price
    float total;                   // price * quantity

} Purchase;

/* Holds information about a customer. */
typedef struct {
    char name[MAX_NAME_LEN+1];         // name
    Purchase purchases[MAX_PURCHASES]; // all purchases by the customer
    int num_purchases;                 // number of purchases
    float total;                       // total money spent by customer

} Customer;

/* Adds a customer to an array of customers. */
void add_customer(Customer customers[], int *num_customers, char *customer_name) {
    //check if a customer can be added to the array
    //+ 1 because customer count is increased after addition to the array
    if(*num_customers + 1> MAX_CUSTOMERS) {
        printf("Database is full; cannot add another customer!\n");
        exit(1);  // quit the program if too many customers are being added
    }

    strcpy(customers[*num_customers].name, customer_name); // initialize customer members
    customers[*num_customers].num_purchases = 0;
    customers[*num_customers].total = 0;
    *num_customers += 1;
}

/* Looks for a customer in an array of customers.
   If a customer is found returns the index of the customer,
   otherwise returns -1.
*/
int find_customer(Customer customers[], int num_customers, char customer_name[]) {
    for(int i = 0; i < num_customers; i++) {
        if(strcmp(customers[i].name, customer_name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Creates a purchase. */
void create_purchase(Purchase *purchase, char *name, int quantity, float price) {
    strcpy(purchase->item_n, name);
    purchase->item_q = quantity;
    purchase->item_p = price;
    purchase->total = purchase->item_p * purchase->item_q;
}

/* Adds a purchase to a customer. */
void add_purchase(Customer *customer, Purchase purchase) {
    if(customer->num_purchases + 1 > MAX_PURCHASES) {
        printf("Database is full; cannot add another purchase!\n");
        exit(1);
    }
    customer->purchases[customer->num_purchases] = purchase;
    customer->total += purchase.total; 
    customer->num_purchases += 1;
}

/* Reads a file containing customers and their orders.
   Adds customers and their purchases.
*/
void read_file(Customer customers[], FILE *infile, int *num_customers) {
    char name[MAX_NAME_LEN + 1];
    char item_name[MAX_NAME_LEN + 1];
    Purchase purchase;
    int quantity;
    float price;

    while (fscanf(infile, "%s %d %s $%f", name, &quantity, item_name, &price) != EOF) {
        int customer_idx = find_customer(customers ,*num_customers, name);
        if(customer_idx == -1) { 
            add_customer(customers, num_customers,name); //add the customer since they couldn't be found
            customer_idx = *num_customers - 1; //the index of the customer is - 1 from the number of customers
        }
        add_purchase(&customers[customer_idx], purchase);
    }
}

/* Given a file pointer checks if the file was opened, quits the programs if not. */
void check_file_open(FILE *file) {
    if(file == NULL) {
        printf("Cannot open file!");
        exit(1);
    }
}

/* Sorts the customers based on how much money they spent; descending.
*/
void sort_customers(Customer customers[], int num_customers) {
    for(int pass = 0; pass < num_customers; pass++) {
        for(int i = 0; i < num_customers - 1; i++) {
            if(customers[i + 1].total > customers[i].total) {
                Customer temp = customers[i];
                customers[i] = customers[i + 1];
                customers[i + 1] = temp;
            }
        }
    }
}

/* Sorts the purchases of a single customer based on the total
   purchase value; descending.
*/
void sort_customer_purchases(Customer *customer) {
    int num_orders = customer->num_purchases;
    for(int pass = 0; pass < num_orders; pass++) {
        for(int i = 0; i < num_orders - 1; i++) {
            if(customer->purchases[i + 1].total > customer->purchases[i].total) {
                Purchase temp = customer->purchases[i];
                customer->purchases[i] = customer->purchases[i + 1];
                customer->purchases[i + 1] = temp;
            }
        }
    }

}

/* Sorts the purchases of all customers based on the total
   purchase value; descending.
*/
void sort_all_purchases(Customer customers[], int num_customers) {
    for(int i = 0; i < num_customers; i++) {
        sort_customer_purchases(&customers[i]);
    }
}

/* Writes customers and their purchases in a chronological order of their first appearance.
   "Customer Name"
   "Item Name" "Item Quantity" $"Single Price"
 */
void write_time_file(Customer customers[], int num_customers) {
    FILE *output_file = fopen("hw4time.txt", "w");
    for(int i = 0; i < num_customers; i++){
        fprintf(output_file, "%s\n", customers[i].name);

        for(int j = 0; j < customers[i].num_purchases; j++) {
                fprintf(output_file,"%s %d $%.2f\n",
                        customers[i].purchases[j].item_n,
                        customers[i].purchases[j].item_q,
                        customers[i].purchases[j].item_p);
        }
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}

/* Writes customers and their purchases based on how much money the customer spent; descending.
   Purchases are ordered in the same way.
   "Name", Total Order = $XXXX "Item Name" "Item Quantity" $"Single Item Price", Item Value = $"Single Price * Quantity"
*/
void write_value_file(Customer customers[], int num_customers) {
    FILE *output_file = fopen("hw4money.txt", "w");
    for(int i = 0; i < num_customers; i++) {
        int num_purchases = customers[i].num_purchases;
        Customer customer = customers[i];
        fprintf(output_file, "%s, Total Order = $%.2f\n",
                customer.name,
                customer.total);

        for(int j = 0; j < num_purchases; j++) {
            Purchase purchase = customer.purchases[j];
            fprintf(output_file, "%s %d $%.2f, Item Value = $%.2f\n",
                    purchase.item_n,
                    purchase.item_q,
                    purchase.item_p,
                    purchase.total);
        }
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}

int main(void) {
    Customer customers[MAX_CUSTOMERS];
    int num_customers = 0;
    FILE *infile = fopen("hw4input.txt", "r");
    check_file_open(infile);
    read_file(customers, infile, &num_customers);
    fclose(infile);
    write_time_file(customers, num_customers);
    sort_customers(customers,num_customers);
    sort_all_purchases(customers, num_customers);
    write_value_file(customers, num_customers);
    return 0;
}
