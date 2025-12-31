#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS  50
#define MAX_LENGTH    100
#define XOR_KEY       0x5A    // simple single-byte key (very weak!)

typedef struct {
    char site[MAX_LENGTH];
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];  // will be XOR-ed
    int used;
} Account;

Account accounts[MAX_ACCOUNTS];
int account_count = 0;

// Very simple XOR "encryption" (same key for encrypt & decrypt)
void xor_crypt(char* str, int len) {
    for(int i = 0; i < len; i++) {
        if(str[i] == '\0') break;
        str[i] = str[i] ^ XOR_KEY;
    }
}

void add_account() {
    if(account_count >= MAX_ACCOUNTS) {
        printf("Storage is full!\n");
        return;
    }

    Account* acc = &accounts[account_count];

    printf("\nEnter website/app name: ");
    fgets(acc->site, MAX_LENGTH, stdin);
    acc->site[strcspn(acc->site, "\n")] = 0;  // remove newline

    printf("Enter username/email: ");
    fgets(acc->username, MAX_LENGTH, stdin);
    acc->username[strcspn(acc->username, "\n")] = 0;

    printf("Enter password: ");
    fgets(acc->password, MAX_LENGTH, stdin);
    acc->password[strcspn(acc->password, "\n")] = 0;

    // "Encrypt" (just XOR)
    xor_crypt(acc->password, MAX_LENGTH);

    acc->used = 1;
    account_count++;

    printf("\nAccount saved (weakly protected)!\n");
}

void show_all() {
    if(account_count == 0) {
        printf("\nNo accounts stored yet.\n");
        return;
    }

    printf("\n=== Stored Accounts ===\n");
    for(int i = 0; i < account_count; i++) {
        if(!accounts[i].used) continue;

        Account acc = accounts[i];
        
        // Make copy because we don't want to modify original
        char pass_copy[MAX_LENGTH];
        strncpy(pass_copy, acc.password, MAX_LENGTH);
        
        // "Decrypt" (same operation as encrypt with XOR)
        xor_crypt(pass_copy, MAX_LENGTH);

        printf("\n%d) %s\n", i+1, acc.site);
        printf("   User: %s\n", acc.username);
        printf("   Pass: %s\n", pass_copy);
    }
}

void save_to_file(const char* filename) {
    FILE* f = fopen(filename, "wb");
    if(!f) {
        printf("Cannot create file!\n");
        return;
    }

    fwrite(&account_count, sizeof(int), 1, f);
    fwrite(accounts, sizeof(Account), MAX_ACCOUNTS, f);
    fclose(f);
    printf("Saved to %s\n", filename);
}

void load_from_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if(!f) {
        printf("No save file found.\n");
        return;
    }

    fread(&account_count, sizeof(int), 1, f);
    fread(accounts, sizeof(Account), MAX_ACCOUNTS, f);
    fclose(f);
    printf("Loaded %d accounts from file.\n", account_count);
}

int main() {
    char choice;
    const char* filename = "my_passwords.dat";

    // Try to load previous data
    load_from_file(filename);

    do {
        printf("\n");
        printf("1 - Add new account\n");
        printf("2 - Show all passwords\n");
        printf("3 - Save & Exit\n");
        printf("4 - Exit without saving\n");
        printf("Choice: ");

        choice = getchar();
        while(getchar() != '\n');  // clear input buffer

        switch(choice) {
            case '1':
                add_account();
                break;
            case '2':
                show_all();
                break;
            case '3':
                save_to_file(filename);
                printf("Goodbye!\n");
                return 0;
            case '4':
                printf("Goodbye! (changes not saved)\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    } while(1);

    return 0;
}