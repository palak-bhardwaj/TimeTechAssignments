#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define NAME_LENGTH 50

typedef struct {
    int productId;
    char productName[NAME_LENGTH];
    float price;
    int quantity;
} Product;

bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isValidName(char *name) {
    int i = 0;
    while (name[i] != '\0') {
        if (!isLetter(name[i])) return false;
        i++;
    }
    return true;
}

int strLength(char *str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

void strCopy(char *dest, char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

char toLowerCase(char c) {
    return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

bool equalsIgnoreCase(char *a, char *b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (toLowerCase(a[i]) != toLowerCase(b[i])) return false;
        i++;
    }
    return a[i] == b[i];
}

bool isSubstringIgnoreCase(char *str, char *sub) {
    int n = strLength(str), m = strLength(sub);
    for (int i = 0; i <= n - m; i++) {
        bool match = true;
        for (int j = 0; j < m; j++) {
            if (toLowerCase(str[i + j]) != toLowerCase(sub[j])) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

bool isDuplicateId(Product *products, int productCount, int id) {
    for (int i = 0; i < productCount; i++)
        if (products[i].productId == id)
            return true;
    return false;
}

bool isDuplicateName(Product *products, int productCount, char *name) {
    for (int i = 0; i < productCount; i++)
        if (equalsIgnoreCase(products[i].productName, name))
            return true;
    return false;
}

void addProduct(Product **products, int *productCount) {
    int id, quantity;
    float price;
    char name[NAME_LENGTH];
    bool valid = false;

    while (!valid) {
        printf("Product ID: ");
        scanf("%d", &id);
        if (id <= 0)
            printf("Invalid ID.\n");
        else if (isDuplicateId(*products, *productCount, id))
            printf("Duplicate ID.\n");
        else
            valid = true;
    }

    valid = false;
    while (!valid) {
        printf("Product Name: ");
        scanf("%s", name);
        if (!isValidName(name))
            printf("Invalid Name.\n");
        else if (isDuplicateName(*products, *productCount, name))
            printf("Duplicate Name.\n");
        else
            valid = true;
    }

    valid = false;
    while (!valid) {
        printf("Product Price: ");
        scanf("%f", &price);
        if (price <= 0)
            printf("Invalid Price.\n");
        else
            valid = true;
    }

    valid = false;
    while (!valid) {
        printf("Product Quantity: ");
        scanf("%d", &quantity);
        if (quantity <= 0)
            printf("Invalid Quantity.\n");
        else
            valid = true;
    }

    (*productCount)++;
    *products = realloc(*products, (*productCount) * sizeof(Product));

    Product *p = &(*products)[*productCount - 1];
    p->productId = id;
    strCopy(p->productName, name);
    p->price = price;
    p->quantity = quantity;

    printf("Product added successfully!\n");
}

void viewAllProducts(Product *products, int productCount) {
    if (productCount == 0) {
        printf("No products available.\n");
        return;
    }
    for (int i = 0; i < productCount; i++) {
        printf("ID: %d | Name: %s | Price: %.2f | Qty: %d\n",
               products[i].productId, products[i].productName,
               products[i].price, products[i].quantity);
    }
}

void updateQuantity(Product *products, int productCount) {
    int id, q;
    bool found = false;
    printf("Enter Product ID: ");
    scanf("%d", &id);

    for (int i = 0; i < productCount; i++) {
        if (products[i].productId == id) {
            while (true) {
                printf("New Quantity: ");
                scanf("%d", &q);
                if (q > 0) break;
                printf("Invalid quantity.\n");
            }
            products[i].quantity = q;
            printf("Quantity updated successfully.\n");
            found = true;
            break;
        }
    }
    if (!found) printf("Product not found.\n");
}

void searchById(Product *products, int productCount) {
    int id;
    bool found = false;
    printf("Enter Product ID: ");
    scanf("%d", &id);
    for (int i = 0; i < productCount; i++)
        if (products[i].productId == id) {
            printf("ID: %d | Name: %s | Price: %.2f | Qty: %d\n",
                   products[i].productId, products[i].productName,
                   products[i].price, products[i].quantity);
            found = true;
        }
    if (!found) printf("Product not found.\n");
}

void searchByName(Product *products, int productCount) {
    char name[NAME_LENGTH];
    bool found = false;
    printf("Enter Name (partial allowed): ");
    scanf("%s", name);
    for (int i = 0; i < productCount; i++)
        if (isSubstringIgnoreCase(products[i].productName, name)) {
            printf("ID: %d | Name: %s | Price: %.2f | Qty: %d\n",
                   products[i].productId, products[i].productName,
                   products[i].price, products[i].quantity);
            found = true;
        }
    if (!found) printf("No matching products.\n");
}

void searchByPriceRange(Product *products, int productCount) {
    float min, max;
    bool found = false;
    printf("Enter minimum price: ");
    scanf("%f", &min);
    printf("Enter maximum price: ");
    scanf("%f", &max);

    for (int i = 0; i < productCount; i++)
        if (products[i].price >= min && products[i].price <= max) {
            printf("ID: %d | Name: %s | Price: %.2f | Qty: %d\n",
                   products[i].productId, products[i].productName,
                   products[i].price, products[i].quantity);
            found = true;
        }
    if (!found) printf("No products in range.\n");
}

void deleteProduct(Product **products, int *productCount) {
    int id, index = -1;
    printf("Enter Product ID: ");
    scanf("%d", &id);

    for (int i = 0; i < *productCount; i++)
        if ((*products)[i].productId == id)
            index = i;

    if (index == -1) {
        printf("Product not found.\n");
        return;
    }

    for (int i = index; i < *productCount - 1; i++)
        (*products)[i] = (*products)[i + 1];

    (*productCount)--;
    *products = realloc(*products, (*productCount) * sizeof(Product));
    printf("Product deleted.\n");
}

void runSystem() {
    int n;
    printf("Enter initial number of products: ");
    scanf("%d", &n);

    Product *products = calloc(n, sizeof(Product));
    int productCount = 0; 

    for (int i = 0; i < n; i++) {
        printf("\nEnter details for product %d:\n", i + 1);
        addProduct(&products, &productCount);
    }

    int choice;
    do {
        printf("\n========= INVENTORY MENU =========\n");
        printf("1. Add Product\n2. View All\n3. Update Quantity\n4. Search by ID\n5. Search by Name\n6. Search by Price Range\n7. Delete Product\n8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addProduct(&products, &productCount); break;
            case 2: viewAllProducts(products, productCount); break;
            case 3: updateQuantity(products, productCount); break;
            case 4: searchById(products, productCount); break;
            case 5: searchByName(products, productCount); break;
            case 6: searchByPriceRange(products, productCount); break;
            case 7: deleteProduct(&products, &productCount); break;
            case 8: printf("Memory released successfully. Exiting program...\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 8);

    free(products);
}
int main() {
    runSystem();
    return 0;
}
