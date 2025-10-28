#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 50

typedef struct {
    int productId;
    char productName[NAME_LENGTH];
    float price;
    int quantity;
} Product;

void addProduct(Product **products, int *productCount) {
    (*productCount)++;
    *products = realloc(*products, (*productCount) * sizeof(Product));
    if (*products == NULL) {
        printf("Memory reallocation failed!\n");
        exit(1);
    }

    Product *newProduct = &(*products)[*productCount - 1];

    printf("\nEnter new product details:\n");
    printf("Product ID: ");
    scanf("%d", &newProduct->productId);
    printf("Product Name: ");
    scanf("%s", newProduct->productName);
    printf("Product Price: ");
    scanf("%f", &newProduct->price);
    printf("Product Quantity: ");
    scanf("%d", &newProduct->quantity);

    printf("Product added successfully!\n");
}

void viewAllProducts(Product *products, int productCount) {
    if (productCount == 0) {
        printf("No products available.\n");
        return;
    }

    printf("\n========= PRODUCT LIST =========\n");
    for (int i = 0; i < productCount; i++) {
        printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               products[i].productId, products[i].productName,
               products[i].price, products[i].quantity);
    }
}

void updateQuantity(Product *products, int productCount) {
    int id, found = 0, newQuantity;

    printf("Enter Product ID to update quantity: ");
    scanf("%d", &id);

    for (int i = 0; i < productCount; i++) {
        if (products[i].productId == id) {
            printf("Enter new Quantity: ");
            scanf("%d", &newQuantity);
            products[i].quantity = newQuantity;
            printf("Quantity updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Product not found!\n");
}

void searchById(Product *products, int productCount) {
    int id, found = 0;

    printf("Enter Product ID to search: ");
    scanf("%d", &id);

    for (int i = 0; i < productCount; i++) {
        if (products[i].productId == id) {
            printf("Product Found:\n");
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].productId, products[i].productName,
                   products[i].price, products[i].quantity);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Product not found!\n");
}

void searchByName(Product *products, int productCount) {
    char name[NAME_LENGTH];
    int found = 0;

    printf("Enter name to search (partial allowed): ");
    scanf("%s", name);

    printf("Products Found:\n");
    for (int i = 0; i < productCount; i++) {
        if (strstr(products[i].productName, name)) {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].productId, products[i].productName,
                   products[i].price, products[i].quantity);
            found = 1;
        }
    }

    if (!found)
        printf("No products found with the given name.\n");
}

void searchByPriceRange(Product *products, int productCount) {
    float minPrice, maxPrice;
    int found = 0;

    printf("Enter minimum price: ");
    scanf("%f", &minPrice);
    printf("Enter maximum price: ");
    scanf("%f", &maxPrice);

    printf("Products in price range:\n");
    for (int i = 0; i < productCount; i++) {
        if (products[i].price >= minPrice && products[i].price <= maxPrice) {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   products[i].productId, products[i].productName,
                   products[i].price, products[i].quantity);
            found = 1;
        }
    }

    if (!found)
        printf("No products found in this price range.\n");
}

void deleteProduct(Product **products, int *productCount) {
    int id, index = -1;

    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < *productCount; i++) {
        if ((*products)[i].productId == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Product not found!\n");
        return;
    }

    for (int i = index; i < *productCount - 1; i++)
        (*products)[i] = (*products)[i + 1];

    (*productCount)--;
    *products = realloc(*products, (*productCount) * sizeof(Product));

    if (*productCount > 0 && *products == NULL) {
        printf("Memory reallocation failed!\n");
        exit(1);
    }

    printf("Product deleted successfully!\n");
}

int main() {
    int productCount;
    Product *products = NULL;
    int choice;

    printf("Enter initial number of products: ");
    scanf("%d", &productCount);

    products = calloc(productCount, sizeof(Product));
    if (products == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    for (int i = 0; i < productCount; i++) {
        printf("\nEnter details for product %d:\n", i + 1);
        printf("Product ID: ");
        scanf("%d", &products[i].productId);
        printf("Product Name: ");
        scanf("%s", products[i].productName);
        printf("Price: ");
        scanf("%f", &products[i].price);
        printf("Quantity: ");
        scanf("%d", &products[i].quantity);
    }

    do {
        printf("\n========= INVENTORY MENU =========\n");
        printf("1. Add New Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search Product by ID\n");
        printf("5. Search Product by Name\n");
        printf("6. Search Product by Price Range\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addProduct(&products, &productCount); 
                    break;
            case 2: viewAllProducts(products, productCount);
                    break;
            case 3: updateQuantity(products, productCount);
                    break;
            case 4: searchById(products, productCount);      
                    break;
            case 5: searchByName(products, productCount);
                    break;
            case 6: searchByPriceRange(products, productCount);
                    break;
            case 7: deleteProduct(&products, &productCount); 
                    break;
            case 8: printf("Memory released successfully. Exiting program...\n");   
                    break;
            default: printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 8);

    free(products);
    return 0;
}
