#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MAX_INVENTORY 10
#define MAX_TESTS 5
#define MAX_CART 15
#define MAX_NAME 100
#define MAX_ADDRESS 250
#define ORDER_FILENAME "orders.txt"

typedef struct {
    int id;
    char name[MAX_NAME];
    float price;
    int stock;
} Medicine;

typedef struct {
    int id;
    char name[MAX_NAME];
    float price;
} BloodTest;

typedef struct {
    char name[MAX_NAME];
    char phone[11]; 
    char address[MAX_ADDRESS];

} User;

typedef struct {
    int id;
    int isTest;
    char name[MAX_NAME];
    int quantity;
    float unitPrice;
} CartItem;

typedef struct {
    long orderId;
    User customer;
    CartItem items[MAX_CART];
    int itemCount;
    float totalAmount;
    char paymentMethod[50]; 
} Order;

void clearScreen();
void clearInputBuffer();
int displayMainMenu();
void displaySplashScreen();
void displayTermsAndConditions(int *accepted);
void getUserDetails(User *user);
void displayMedicines(Medicine *inventory, int count);
void addMedicineToCart(Medicine *inventory, int medCount, CartItem *cart, int *cartCount);
void displayBloodTests(BloodTest *tests, int count);
void addTestToCart(BloodTest *testMenu, int testCount, CartItem *cart, int *cartCount);
void viewCart(CartItem *cart, int cartCount);
float calculateTotal(CartItem *cart, int cartCount);
void saveOrderToFile(Order *order);
void processPayment(char *paymentBuffer, float amount);

void clearScreen() {

    printf("\033[H\033[J");
}

int main() {
    clearScreen();
    displaySplashScreen();
    
    int termsAccepted = 0;
    displayTermsAndConditions(&termsAccepted);
    
    if (!termsAccepted) {
        printf("\nTerms and Conditions not accepted. Exiting the program.\n");
        return 0; 
    }

    Medicine inventory[MAX_INVENTORY] = {
        {101, "Paracetamol 500mg", 30.50, 50},
        {102, "Aspirin 75mg", 15.00, 40},
        {103, "Cough Syrup 100ml", 120.00, 30},
        {104, "Antacid Gel 170ml", 85.75, 25},
        {105, "Vitamin C 500mg", 99.00, 60},
        {106, "Vitamin D 500mg", 150.00, 60}
    };
    int medCount = 6;

    BloodTest testMenu[MAX_TESTS] = {
        {201, "Complete Blood Count (CBC)", 350.00},
        {202, "Thyroid Profile (T3, T4, TSH)", 800.00},
        {203, "Lipid Profile", 900.00},
        {204, "Blood Sugar (Fasting)", 150.00}
    };
    int testCount = 4;

    User customer;
    CartItem cart[MAX_CART];
    int cartCount = 0;
    int choice = 0;

    clearScreen();
    printf("=== Welcome to Quick-Med Pharmacy & Labs ===\n");
    printf("     (Your 10-minute delivery partner)\n\n");
    
    getUserDetails(&customer);

    do {
        choice = displayMainMenu();
        
        switch (choice) {
            case 1:
                clearScreen();
                displayMedicines(inventory, medCount);
                addMedicineToCart(inventory, medCount, cart, &cartCount);
                printf("\nPress Enter to return to menu...");
                clearInputBuffer();
                getchar(); 
                break;
            case 2:
                clearScreen();
                displayBloodTests(testMenu, testCount);
                addTestToCart(testMenu, testCount, cart, &cartCount);
                printf("\nPress Enter to return to menu...");
                clearInputBuffer();
                getchar();
                break;
            case 3: 
                clearScreen();
                viewCart(cart, cartCount);
                printf("Press Enter to return to menu...");
                getchar(); 
                break;
            case 4:
                clearScreen();
                if (cartCount == 0) {
                    printf("Your cart is empty. Please add items before checking out.\n\n");
                    printf("Press Enter to return to menu...");
                    getchar();
                } else {
                    printf("\n=========================================\n");
                    printf("          FINALIZING YOUR ORDER          \n");
                    printf("=========================================\n");
                    Order finalOrder;
                    srand(time(NULL));
                    finalOrder.orderId = time(NULL);
                    finalOrder.customer = customer;
                    memcpy(finalOrder.items, cart, cartCount * sizeof(CartItem));
                    finalOrder.itemCount = cartCount;
                    finalOrder.totalAmount = calculateTotal(cart, cartCount);
                    
                    printf("Total Amount to Pay: %.2f\n", finalOrder.totalAmount);
                    
                    processPayment(finalOrder.paymentMethod, finalOrder.totalAmount);        
                    
                    saveOrderToFile(&finalOrder);                    
                    
                    printf("\nOrder Confirmed!\n");
                    printf("Your Order ID is: %ld\n", finalOrder.orderId);
                    printf("Total Amount: %.2f\n", finalOrder.totalAmount);
                    printf("Payment Mode: %s\n", finalOrder.paymentMethod); 
                    
                    int hasMedicines = 0;
                    int hasTests = 0;
                    for (int i = 0; i < cartCount; i++) {
                        if (cart[i].isTest) {
                            hasTests = 1;
                        } else {
                            hasMedicines = 1;
                        }
                    }

                    printf("\n--- Service Details ---\n");
                    if (hasMedicines && hasTests) {
                        printf("1. MEDICINES: Will be delivered in 10 minutes to: %s\n", finalOrder.customer.address);
                        printf("2. BLOOD TESTS: A specialized Lab Technician will visit your address shortly for sample collection.\n");
                    } 
                    else if (hasTests) {
                        printf("A specialized Lab Technician will visit %s shortly for sample collection.\n", finalOrder.customer.address);
                        printf("You will receive a call to coordinate the exact timing.\n");
                    } 
                    else {
                        printf("Your order will be delivered in 10 minutes to: %s\n", finalOrder.customer.address);
                    }
                    
                    printf("\nThank you for using Quick-Med!\n");
                    choice = 5; 
                }
                break;
            case 5: 
                printf("\nThank you for visiting Quick-Med. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice. Please try again.\n\n");
                printf("Press Enter to continue...");
                clearInputBuffer();
                getchar();
        }
    } while (choice != 5);

    return 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void displayTermsAndConditions(int *accepted) {
    char choice;
    *accepted = 0; 
    printf("\n=======================================================\n");
    printf("          QUICK-MED TERMS & CONDITIONS\n");
    printf("=======================================================\n");
    printf("1. Medicine orders require a valid prescription.\n");
    printf("2. Blood test results will be delivered digitally within 48 hours.\n");
    printf("3. Delivery is guaranteed within the service area only.\n");
    printf("4. All prices are final and include applicable taxes.\n");
    printf("5. Cancellation after payment is subject to a 10%% fee.\n");
    printf("-------------------------------------------------------\n");
    printf("Do you agree to the Terms & Conditions? (Y/N): ");
        if (scanf(" %c", &choice) == 1) { 
        if (toupper(choice) == 'Y') {
            *accepted = 1;
            printf("\nTerms accepted. Proceeding to user details...\n");
        } else {
            *accepted = 0;
        }
    }
    clearInputBuffer(); 
}

void displaySplashScreen() {
    printf("=======================================================\n");
    printf("                  C PROGRAMMING PROJECT\n");
    printf("=======================================================\n");
    printf(" Project Name: Quick-Med Pharmacy & Lab Management System\n");
    printf("-------------------------------------------------------\n");
    printf(" Submitted To: Pankaj Badoni\n");
    printf(" Submitted By: Ayushi Mishra\n");
    printf("=======================================================\n");
    printf("\nPress ENTER to continue to the Terms & Conditions...\n");
    clearInputBuffer();
}

void processPayment(char *paymentBuffer, float amount) {
    int pChoice;
    int valid = 0;
    
    printf("\n--- Select Payment Method ---\n");
    printf("1. Cash on Delivery / Pay at Collection\n");
    printf("2. Credit/Debit Card\n");
    printf("3. UPI / Online Wallet\n");
  
    
    while (!valid) {
        printf("Enter choice (1-3): ");
        if (scanf("%d", &pChoice) == 1) {
            clearInputBuffer();
            switch (pChoice) {
                case 1:
                    strcpy(paymentBuffer, "Cash/Pay on Visit");
                    valid = 1;
                    break;
                case 2:
                    printf("Processing Card Payment...\n");
                    printf("Payment Successful!\n");
                    strcpy(paymentBuffer, "Credit/Debit Card");
                    valid = 1;
                    break;
                case 3:
                    printf("Processing UPI Payment...\n");
                    printf("Payment Successful!\n");
                    strcpy(paymentBuffer, "UPI/Wallet");
                    valid = 1;
                    break;
                default:
                    printf("Invalid payment choice. Please try again.\n");
            }
        } else {
            clearInputBuffer();
            printf("Invalid input. Please enter a number.\n");
        }
    }
}

void getUserDetails(User *user) {
    printf("Please enter your details to begin:\n");
    printf("Full Name: ");
    fgets(user->name, MAX_NAME, stdin);
    user->name[strcspn(user->name, "\n")] = 0;

    while (1) {
        char tempPhone[50]; 
        printf("Phone Number (10 digits): ");
        
        if (fgets(tempPhone, sizeof(tempPhone), stdin) != NULL) {
            tempPhone[strcspn(tempPhone, "\n")] = 0;

            if (strlen(tempPhone) != 10) {
                printf("Error: Phone number must be exactly 10 digits.\n");
                continue; 
            }
            int valid = 1;
            for (int i = 0; i < 10; i++) {
                if (!isdigit(tempPhone[i])) {
                    valid = 0;
                    break;
                }
            }
            if (valid) {
                strcpy(user->phone, tempPhone);
                break;
            } else {
                printf("Error: Invalid character found. Phone number cannot contain letters or special characters.\n");
            }
        }
    }
    printf("Delivery Address: ");
    fgets(user->address, MAX_ADDRESS, stdin);
    user->address[strcspn(user->address, "\n")] = 0;

    printf("\nWelcome, %s! Let's get your order.\n", user->name);
    
    printf("\nPress Enter to go to Main Menu...");
    getchar(); 
}

int displayMainMenu() {
    clearScreen(); 
    int choice = 0;
    printf("========================================\n");
    printf("          QUICK-MED MAIN MENU            \n");
    printf("========================================\n");
    printf("1. Order Medicines\n");
    printf("2. Book a Blood Test\n");
    printf("3. View Cart\n");
    printf("4. Checkout & Place Order\n");
    printf("5. Exit\n");
    printf("----------------------------------------\n");
    printf("Enter your choice: ");
    if (scanf("%d", &choice) != 1) {
        choice = -1; 
    }
    clearInputBuffer();
    return choice;
}

void displayMedicines(Medicine *inventory, int count) {
    printf("\n--- Available Medicines ---\n");
    printf("ID   | Name                   | Price   | Stock\n");
    printf("-----|----------------------|---------|-------\n");
    for (int i = 0; i < count; i++) {
        printf("%-4d | %-20.20s | %-7.2f | %d\n",
               inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].stock);
    }
    printf("\n");
}

void displayBloodTests(BloodTest *tests, int count) {
    printf("\n--- Available Blood Tests ---\n");
    printf("ID   | Name                           | Price\n");
    printf("-----|-----------------------------|---------\n");
    for (int i = 0; i < count; i++) {
        printf("%-4d | %-27.27s | %.2f\n",
               tests[i].id, tests[i].name, tests[i].price);
    }
    printf("\n");
}

void addMedicineToCart(Medicine *inventory, int medCount, CartItem *cart, int *cartCount) {
    int id, quantity;
    printf("Enter Medicine ID to add to cart: ");
    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("Invalid ID.\n");
        return;
    }
    int foundIndex = -1;
    for (int i = 0; i < medCount; i++) {
        if (inventory[i].id == id) {
            foundIndex = i;
            break;
        }
    }
    if (foundIndex == -1) {
        printf("Invalid Medicine ID.\n");
        clearInputBuffer();
        return;
    }
    printf("Enter quantity (Stock: %d): ", inventory[foundIndex].stock);
    if (scanf("%d", &quantity) != 1) {
        clearInputBuffer();
        printf("Invalid quantity.\n");
        return;
    }
    clearInputBuffer();
    if (quantity <= 0) {
        printf("Quantity must be at least 1.\n");
    } else if (quantity > inventory[foundIndex].stock) {
        printf("Sorry, only %d units are in stock.\n", inventory[foundIndex].stock);
    } else if (*cartCount >= MAX_CART) {
        printf("Your cart is full!\n");
    } else {
        CartItem *newItem = &cart[*cartCount];
        newItem->id = inventory[foundIndex].id;
        newItem->isTest = 0;
        strcpy(newItem->name, inventory[foundIndex].name);
        newItem->quantity = quantity;
        newItem->unitPrice = inventory[foundIndex].price;
        inventory[foundIndex].stock -= quantity;
        (*cartCount)++;
        printf("Added %d x %s to your cart.\n", quantity, newItem->name);
    }
}

void addTestToCart(BloodTest *testMenu, int testCount, CartItem *cart, int *cartCount) {
    int id;
    printf("Enter Test ID to book: ");
    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("Invalid ID.\n");
        return;
    }
    clearInputBuffer();
    int foundIndex = -1;
    for (int i = 0; i < testCount; i++) {
        if (testMenu[i].id == id) {
            foundIndex = i;
            break;
        }
    }
    if (foundIndex == -1) {
        printf("Invalid Test ID.\n");
        return;
    }
    if (*cartCount >= MAX_CART) {
        printf("Your cart is full!\n");
    } else {
        CartItem *newItem = &cart[*cartCount];
        newItem->id = testMenu[foundIndex].id;
        newItem->isTest = 1;
        strcpy(newItem->name, testMenu[foundIndex].name);
        newItem->quantity = 1;
        newItem->unitPrice = testMenu[foundIndex].price;
        (*cartCount)++;
        printf("Added '%s' to your cart.\n", newItem->name);
    }
}

float calculateTotal(CartItem *cart, int cartCount) {
    float total = 0.0;
    for (int i = 0; i < cartCount; i++) {
        total += cart[i].unitPrice * cart[i].quantity;
    }
    return total;
}

void viewCart(CartItem *cart, int cartCount) {
    if (cartCount == 0) {
        printf("\nYour cart is empty.\n\n");
        return;
    }
    
    printf("\n--- Your Cart (%d items) ---\n", cartCount);
    printf("Type  | Qty | Name                   | Unit Price | Total\n");
    printf("------|-----|----------------------|------------|---------\n");
    for (int i = 0; i < cartCount; i++) {
        printf("%-5s | %-3d | %-20.20s | %-10.2f | %-7.2f\n",
               cart[i].isTest ? "Test" : "Med",
               cart[i].quantity,
               cart[i].name,
               cart[i].unitPrice,
               cart[i].unitPrice * cart[i].quantity);
    }
    float total = calculateTotal(cart, cartCount);
    printf("------------------------------------------------------\n");
    printf("Grand Total: %.2f\n\n", total);
}

void saveOrderToFile(Order *order) {
    FILE *file = fopen(ORDER_FILENAME, "a");
    if (file == NULL) {
        printf("CRITICAL ERROR: Could not save order to file '%s'.\n", ORDER_FILENAME);
        return;
    }
    time_t t = time(NULL);
    char *time_str = ctime(&t);
    time_str[strcspn(time_str, "\n")] = 0;
    
    fprintf(file, "===================================================\n");
    fprintf(file, "Order ID: %ld\n", order->orderId);
    fprintf(file, "Timestamp: %s\n", time_str);
    fprintf(file, "\n--- Customer Details ---\n");
    fprintf(file, "Name: %s\n", order->customer.name);
    fprintf(file, "Phone: %s\n", order->customer.phone);
    fprintf(file, "Address: %s\n", order->customer.address);
    fprintf(file, "\n--- Order Items (%d) ---\n", order->itemCount);
    for (int i = 0; i < order->itemCount; i++) {
        fprintf(file, "  - [%s] %s (Qty: %d @ %.2f each)\n",
                order->items[i].isTest ? "TEST" : "MED",
                order->items[i].name,
                order->items[i].quantity,
                order->items[i].unitPrice);
    }
    fprintf(file, "\n--- Total Amount: %.2f ---\n", order->totalAmount);
    fprintf(file, "Payment Method: %s\n", order->paymentMethod); 
    fprintf(file, "===================================================\n\n");
    fclose(file);
    
    printf("Order details saved to %s\n", ORDER_FILENAME);
}
