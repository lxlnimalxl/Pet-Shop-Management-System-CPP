#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <map>
#include <limits>
#include <cctype>
#include <thread>
#include <chrono>
#include <sstream>
#include <cstdio>
#include <stdexcept>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#undef GetUserName
#else
#include <sys/stat.h>
#endif

using namespace std;

//تابع برای تاخیر انیمشین
//chorono برای مدیریت زمان استفاده میشه
void animateDelay(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

// تابع برای نمایش متن با انیمیشن تایپ
void typeWriter(const string& text, int delay = 30) {
    for (char c : text) {
        //flush برای نمایش فوری خروجی
        cout << c << flush;
        animateDelay(delay);
    }
    cout << endl;
}

//تابع پاک کردن صفحه cmd 
void clearCmd() {
    //پیش‌پردازنده‌های شرطی #ifdef #else #endif
    //بررسی ویندوز 32 و 64 با _WIN32
#ifdef _WIN32
    //کتابخونه cstdlib برای اجرای دستورات cmd 
    //cls برای پاک کردن در ویندوز
    system("cls");
#else 
    //برای لینوکس
    system("clear");
#endif
}

//تابع نوشتن اسم برنامه
void showName(const string& title) {
    //فراخوانی تابع پاک کردن صفحه برای زیبایی کار
    clearCmd();
    cout << "|------------------------------------|" << endl;
    // سایتhttps://www.asciiart.eu/
    cout << "|___  ____ ___    ____ _  _ ____ ___ |" << endl;
    cout << "||__] |___  |     [__  |__| |  | |__]|" << endl;
    cout << "||    |___  |     ___] |  | |__| |   |" << endl;
    cout << "|                                    |" << endl;
    cout << "|------------------------------------|" << endl;
    typeWriter(title, 20);
}

namespace {
const string passwordFileHeader = "# Pet Shop password hashes v1";
const int passwordSaltSize = 16;
const int passwordHashSize = 32;
const int passwordIterations = 600000;

string toHex(const unsigned char* bytes, int size) {
    const string digits = "0123456789abcdef";
    string hex;
    for (int i = 0; i < size; ++i) {
        hex += digits[bytes[i] >> 4];
        hex += digits[bytes[i] & 15];
    }
    return hex;
}

string randomHex() {
    unsigned char bytes[passwordSaltSize];
    if (RAND_bytes(bytes, sizeof(bytes)) != 1) {
        return "";
    }
    return toHex(bytes, sizeof(bytes));
}

// Use the encoded salt directly as the PBKDF2 salt so the stored value can be reused as-is.
string derivePasswordHash(const string& password, const string& salt) {
    unsigned char hash[passwordHashSize];
    if (password.size() > static_cast<size_t>(numeric_limits<int>::max()) ||
        PKCS5_PBKDF2_HMAC(password.data(), static_cast<int>(password.size()),
            reinterpret_cast<const unsigned char*>(salt.data()), static_cast<int>(salt.size()),
            passwordIterations, EVP_sha256(), sizeof(hash), hash) != 1) {
        throw runtime_error("Could not hash the password.");
    }
    return toHex(hash, sizeof(hash));
}

string hashPassword(const string& password) {
    string salt = randomHex();
    if (salt.empty()) {
        throw runtime_error("Could not generate a password salt.");
    }
    return salt + "$" + derivePasswordHash(password, salt);
}

bool verifyPassword(const string& password, const string& passwordHash) {
    const size_t saltLength = 2 * passwordSaltSize;
    const size_t hashLength = 2 * passwordHashSize;
    if (passwordHash.size() != saltLength + 1 + hashLength || passwordHash[saltLength] != '$') {
        return false;
    }

    string hash = derivePasswordHash(password, passwordHash.substr(0, saltLength));
    return CRYPTO_memcmp(hash.data(), passwordHash.data() + saltLength + 1, hashLength) == 0;
}

bool replaceFile(const string& source, const string& destination) {
#ifdef _WIN32
    return MoveFileExA(source.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
#else
    return rename(source.c_str(), destination.c_str()) == 0;
#endif
}
}

class User {
protected:
    string username;
    string passwordHash;
    string role;
public:
    //سازنده
    User(const string& UserName, const string& PasswordHash, const string& Role = "user") {
        username = UserName;
        passwordHash = PasswordHash;
        role = Role;
    }

    //ستر
    void SetUserName(const string& UserName) {
        username = UserName;
    }

    void SetRole(const string& Role) {
        role = Role;
    }

    //مخرب
    virtual ~User() {}

    //گتر
    string GetUserName() const {
        return username;
    }
    const string& GetPasswordHash() const {
        return passwordHash;
    }
    string GetRole() const {
        return role;
    }
    //تابع مجازی برای نمایش منو کاربر
    virtual void displayMenu() = 0;
};

class Customer : public User {
private:
    double balance;
    vector<string> orders;
public:
    //سازنده
    Customer(const string& UserName, const string& PasswordHash, double Balance = 0.0) :
        User(UserName, PasswordHash, "customer"), balance(Balance) {
    }
    //ستر
    void SetBalance(double Balance) {
        balance = Balance;
    }
    void SetOreders(vector<string>& Oreders) {
        orders = Oreders;
    }
    //گتر
    double GetBalance() const {
        return balance;
    }
    const vector<string>& GetOreders() const {
        return orders;
    }
    //تابع شارژ اکانت 
    void chargeAccount(double amount) {
        // اصلاح باگ: بررسی مقدار منفی برای شارژ حساب
        if (amount < 0) {
            typeWriter("Amount cannot be negative!", 50);
            animateDelay(1200);
            return;
        }
        balance += amount;
        cout << endl;
        typeWriter("Account charged successfully!", 30);
        cout << "New Balance: " << balance << endl;
        animateDelay(1500);
    }
    //تابع اضافه کردن سفارش
    void addOrder(const string& product) {
        orders.push_back(product);
    }
    // تابع خرید
    bool makePurchase(double amount) {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }
    //منوی مشتری
    void displayMenu() override;
};

class Seller : public User {
public:
    //سازنده
    Seller(const string& UserName, const string& PasswordHash) :
        User(UserName, PasswordHash, "seller") {
    }
    //نمایش منو فرشنده
    void displayMenu() override;
};

class Product {
private:
    string name;
    string category;
    string suitablefor;
    double price;
public:
    //سازنده
    Product(const string& Name, const string& Category, const string& Suitablefor, double Price) :
        name(Name), category(Category), suitablefor(Suitablefor), price(Price) {
    }
    //ستر
    void SetName(const string& Name) {
        name = Name;
    }
    void SetCategory(const string& Category) {
        category = Category;
    }
    void SetSuitablefor(const string& Suitablefor) {
        suitablefor = Suitablefor;
    }
    void SetPrice(double Price) {
        price = Price;
    }
    //گتر
    string GetName() const {
        return name;
    }
    string GetCategory() const {
        return category;
    }
    string GetSuitablefor() const {
        return suitablefor;
    }
    double GetPrice() const {
        return price;
    }
    //تابع منو
    void display() const {
        //استفاده از کتابخونه iomanip
        //setw(n)عرض کاراکتر
        cout << "  " << left << setw(25) << name << " | " << setw(15) << category << " | " << setw(20) << suitablefor << " | ";
        cout << fixed << setprecision(2) << price << " Toman" << endl;
        cout.unsetf(ios_base::fixed); // اصلاح باگ: بازنشانی تنظیمات فرمت
    }
};

class PetShopSystem {
private:
    //نقش * اشاره گر به اشیا یوزر هست وگرنه خطای push_back میده
    vector<User*> users;
    vector<Product> products;
    string customersFile = "customers.txt";
    string sellersFile = "sellers.txt";
    string productsFile = "products.txt";
    static PetShopSystem* instance;

    // اصلاح باگ: تابع برای مدیریت کاما در ذخیره‌سازی
    string escapeCommas(const string& input) {
        string result = input;
        replace(result.begin(), result.end(), ',', ';');
        return result;
    }

    string unescapeCommas(const string& input) {
        string result = input;
        replace(result.begin(), result.end(), ';', ',');
        return result;
    }

    //تابع بارگزاری کاربران
    void LoadUsers(const string& fileName, const string& role) {
        ofstream createFile(fileName, ios::app);
        createFile.close();
        ifstream file(fileName);
        if (!file) {
            throw runtime_error("Could not open " + fileName + ".");
        }

        string line;
        bool hasHashes = false;
        bool hasAccounts = false;
        while (getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.find_first_not_of(" \t") == string::npos) {
                continue;
            }
            if (!hasAccounts && line == passwordFileHeader) {
                hasHashes = true;
                continue;
            }

            istringstream record(line);
            string username, credential;
            if (!(record >> username >> credential)) {
                throw runtime_error("Invalid account record in " + fileName + ".");
            }
            string passwordHash = hasHashes ? credential : hashPassword(credential);
            if (role == "customer") {
                double balance;
                if (!(record >> balance)) {
                    throw runtime_error("Invalid customer balance in " + fileName + ".");
                }
                vector<string> orders;
                string order;
                record >> ws;
                while (getline(record, order, ' ')) {
                    if (!order.empty()) orders.push_back(unescapeCommas(order));
                }
                Customer* customer = new Customer(username, passwordHash, balance);
                customer->SetOreders(orders);
                users.push_back(customer);
            }
            else {
                if (!(record >> ws).eof()) {
                    throw runtime_error("Invalid seller record in " + fileName + ".");
                }
                users.push_back(new Seller(username, passwordHash));
            }
            hasAccounts = true;
        }
        if (file.bad()) {
            throw runtime_error("Could not read " + fileName + ".");
        }
        file.close();

        if (hasAccounts && !hasHashes && !saveUsers(fileName, role)) {
            throw runtime_error("Could not upgrade passwords in " + fileName + ".");
        }
    }

    bool saveUsers(const string& fileName, const string& role) {
        string suffix = randomHex();
        if (suffix.empty()) {
            return false;
        }
        string tempName = fileName + "." + suffix + ".tmp";
        ofstream file(tempName);
        if (!file) {
            return false;
        }
#ifndef _WIN32
        if (chmod(tempName.c_str(), S_IRUSR | S_IWUSR) != 0) {
            file.close();
            remove(tempName.c_str());
            return false;
        }
#endif

        file << passwordFileHeader << '\n';
        file << setprecision(numeric_limits<double>::max_digits10);
        for (const auto& user : users) {
            if (user->GetRole() != role) {
                continue;
            }
            file << user->GetUserName() << " " << user->GetPasswordHash();
            if (role == "customer") {
                const Customer* customer = static_cast<const Customer*>(user);
                file << " " << customer->GetBalance();
                for (const auto& order : customer->GetOreders()) {
                    file << " " << escapeCommas(order);
                }
            }
            file << '\n';
        }
        file.close();

        if (file && replaceFile(tempName, fileName)) {
            return true;
        }
        remove(tempName.c_str());
        return false;
    }
    //تابع بارگزاری اطلاعات محصولات
    void LoadProduct() {
        // اصلاح باگ: ایجاد فایل خالی در صورت عدم وجود
        ofstream createProdFile(productsFile, ios::app);
        createProdFile.close();

        ifstream prodFile(productsFile);
        //شرطی که نگاه کنه فایل به درستی باز شده یا نه
        if (prodFile) {
            string line;
            //بررسی خط به خط
            while (getline(prodFile, line)) {
                //از کتابخونه cstddef برای استفاده میکنیم 
                //size_t برای پیدا کردن موقعیت کاما ها از  استفاده میکنیم
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                //شرطی که بررسی میکنه هر سه کاما پیدا شده باشد
                //string::npos = مقداری که نشان‌دهنده عدم وجود کاماست
                if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                    // substr = زیررشته‌ای از یک رشته را استخراج می‌کند 
                    string name = unescapeCommas(line.substr(0, pos1)); // اصلاح باگ: مدیریت کاما
                    string category = unescapeCommas(line.substr(pos1 + 1, pos2 - pos1 - 1));
                    string suitablefor = unescapeCommas(line.substr(pos2 + 1, pos3 - pos2 - 1));
                    //stod = تابع تبدیل یک رشته به عدد اعشاری
                    double price = stod(line.substr(pos3 + 1));
                    //emplace_back = یک عنصر جدید را مستقیماً در انتهای وکتور می‌سازد
                    products.emplace_back(name, category, suitablefor, price);
                }
            }
            prodFile.close();
        }
        else {
            // اصلاح باگ: اطلاع‌رسانی در صورت باز نشدن فایل
            typeWriter("Error: Could not open products file!", 50);
            animateDelay(1800);
        }
    }

public:
    //تابع ذخیره سازی کاربران
    void saveUsers() {
        if (!saveUsers(customersFile, "customer")) {
            typeWriter("Error: Could not save customers file!", 50);
        }
        if (!saveUsers(sellersFile, "seller")) {
            typeWriter("Error: Could not save sellers file!", 50);
        }
    }
    //تابع ذخیره سازی محصولات
    void saveProduct() {
        ofstream prodFile(productsFile);
        //پیمایش اطلاعات
        for (const auto& Product : products) {
            //نوشتن اطلاعات
            // اصلاح باگ: مدیریت کاما در ذخیره‌سازی
            prodFile << escapeCommas(Product.GetName()) << "," << escapeCommas(Product.GetCategory()) << ","
                << escapeCommas(Product.GetSuitablefor()) << "," << Product.GetPrice() << endl;
        }
        prodFile.close();
    }

    PetShopSystem() {
        // اصلاح باگ: تنظیم instance
        instance = this;
        showName("Loading System Data...");
        animateDelay(900);
        LoadUsers(customersFile, "customer");
        LoadUsers(sellersFile, "seller");
        LoadProduct();
        typeWriter("System ready!", 80);
        animateDelay(800);
    }
    ~PetShopSystem() {
        showName("Saving Data...");
        animateDelay(900);
        saveUsers();
        saveProduct();
        //حفظ تغییرات محصولات
        for (auto User : users) {
            delete User;
        }
        typeWriter("Data saved successfully!", 80);
        animateDelay(800);
    }

    // اصلاح باگ: اضافه کردن متد GetInstance برای الگوی Singleton
    static PetShopSystem* GetInstance() {
        if (instance == nullptr) {
            instance = new PetShopSystem();
        }
        return instance;
    }

    User* Login() {
        showName("User Login");
        string username, password;
        cout << "  UserName: ";
        cin >> username;
        cout << "  PassWord: ";
        cin >> password;
        //جست و جوی کاربر
        for (auto User : users) {
            if (User->GetUserName() == username && verifyPassword(password, User->GetPasswordHash())) {
                cout << endl;
                typeWriter("Login successful!", 40);
                animateDelay(1200);
                return User;
            }
        }
        cout << endl;
        typeWriter("Invalid username or password!", 50);
        animateDelay(1800);
        //nullptr = نشان دهنده اینکه اشاره گر خالی است
        return nullptr;
    }
    void registerUser() {
        showName("User Registration");
        string username, password, role;
        cout << "  UserName: ";
        cin >> username;
        cout << "  PassWord: ";
        cin >> password;
        cout << "  Role(customer/seller): ";
        cin >> role;
        //تبدیل به حروف کوچک
        //cctype کتابخونه
        for (char& c : role) {
            c = tolower(static_cast<unsigned char>(c));
        }

        //جستجو برای وجود یوزر
        for (const auto& User : users) {
            if (User->GetUserName() == username) {
                cout << endl;
                typeWriter("Username already exists!", 50);
                animateDelay(1800);
                return;
            }
        }

        if (role != "customer" && role != "seller") {
            cout << endl;
            typeWriter("Invalid role!", 50);
            animateDelay(1800);
            return;
        }

        string passwordHash;
        try {
            passwordHash = hashPassword(password);
        }
        catch (const runtime_error& error) {
            typeWriter(string("Error: ") + error.what(), 50);
            animateDelay(1800);
            return;
        }

        bool isCustomer = role == "customer";
        if (isCustomer) {
            users.push_back(new Customer(username, passwordHash));
        }
        else {
            users.push_back(new Seller(username, passwordHash));
        }
        cout << endl;
        if (saveUsers(isCustomer ? customersFile : sellersFile, role)) {
            typeWriter(isCustomer ? "Customer registered successfully!" : "Seller registered successfully!", 50);
        }
        else {
            delete users.back();
            users.pop_back();
            typeWriter("Error: Could not save the new account!", 50);
        }
        animateDelay(1800);
    }

    vector<Product>& GetProduct() {
        return products;
    }

    //اضافه کردن محصول
    void addProduct(const Product& product) {
        // اصلاح باگ: بررسی قیمت منفی
        if (product.GetPrice() < 0) {
            typeWriter("Price cannot be negative!", 50);
            animateDelay(1200);
            return;
        }
        products.push_back(product);
        saveProduct(); // ذخیره‌سازی فوری محصولات
        cout << endl;
        typeWriter("Product added successfully!", 45);
        animateDelay(1200);
    }

    //حدف محصول
    void removeProduct(const string& prodcutName) {
        //کتابخونه algorithm
        //remove_if = لامبادا و پایان و شروع
        auto it = remove_if(products.begin(), products.end(), [&prodcutName](const Product& Pro) {
            return Pro.GetName() == prodcutName;
            });

        if (it != products.end()) {
            products.erase(it, products.end());
            saveProduct(); // ذخیره‌سازی فوری محصولات
            cout << endl;
            typeWriter("Product Removed Successfully.", 50);
        }
        else {
            cout << endl;
            typeWriter("Product Not Found", 50);
        }
        animateDelay(1200);
    }

    //لیست مشتریان
    vector<User*> getCustomers() const {
        vector<User*> customers;
        for (const auto& User : users) {
            if (User->GetRole() == "customer") {
                customers.push_back(User);
            }
        }
        return customers;
    }

    void displayProducts() const {
        //خالی بودن محصولات
        if (products.empty()) {
            cout << endl;
            typeWriter("No products available in the shop.", 50);
            return;
        }

        cout << "\n  " << left << setw(25) << "PRODUCT NAME" << " | " << setw(15) << "CATEGORY" << " | " << setw(20) << "SUITABLE FOR" << " | " << setw(10) << "PRICE" << endl;
        cout << "  " << string(80, '-') << endl;
        //نمایش اطلاعات
        for (const auto& Product : products) {
            Product.display();
            animateDelay(80);
        }
    }
};

// اصلاح باگ: مقداردهی اولیه instance
PetShopSystem* PetShopSystem::instance = nullptr;

//تابع منوی مشتری
void Customer::displayMenu() {
    // اصلاح باگ: مقداردهی system برای جلوگیری از کرش
    PetShopSystem* system = PetShopSystem::GetInstance();

    int choice;
    do {
        showName("CUSTOMER DASHBOARD");
        cout << "  Welcome, " << username << "!\n" << endl;
        cout << "  1. View Products" << endl;
        cout << "  2. Search Products" << endl;
        cout << "  3. Charge Account" << endl;
        cout << "  4. Purchase Product" << endl;
        cout << "  5. View Orders" << endl;
        cout << "  6. View Balance" << endl;
        cout << "  0. Logout" << endl;
        cout << "\n  Enter your choice: ";
        // اصلاح باگ: مدیریت ورودی نامعتبر
        while (!(cin >> choice)) {
            typeWriter("Invalid input! Please enter a number.", 50);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
        case 1: {
            showName("PRODUCT CATALOG");
            system->displayProducts();
            cout << "\nPress Enter to continue...";
            //پاک کردن بافر
            cin.ignore();
            //منتظر موندن برای جواب
            cin.get();
            break;
        }
        case 2: {
            showName("PRODUCT SEARCH");
            string name, category;
            //کتابخونه limits
            //numeric_limits بیشترین مقدار رابرای ما برمیگردونه
            double minPrice = 0, maxPrice = numeric_limits<double>::max();

            cout << "  Product name (leave empty to skip): ";
            cin.ignore();
            getline(cin, name);

            cout << "  Category (leave empty to skip): ";
            getline(cin, category);

            //میمینیم
            cout << "  Minimum price: ";
            string minPriceStr;
            getline(cin, minPriceStr);

            // اصلاح باگ: مدیریت ورودی نامعتبر برای قیمت
            if (!minPriceStr.empty()) {
                try {
                    minPrice = stod(minPriceStr);
                }
                catch (const std::exception& e) {
                    typeWriter("Invalid minimum price! Using default.", 50);
                    minPrice = 0;
                }
            }
            //ماکسیمم
            cout << "  Miximum price: ";
            string maxPriceStr;
            getline(cin, maxPriceStr);
            //.empty = بررسی اینکه شی خالی هست یا نه
            if (!maxPriceStr.empty()) {
                try {
                    maxPrice = stod(maxPriceStr);
                }
                catch (const std::exception& e) {
                    typeWriter("Invalid maximum price! Using default.", 50);
                    maxPrice = numeric_limits<double>::max();
                }
            }

            cout << "\n  " << string(50, '=') << endl;
            cout << "  SEARCH RESULTS" << endl;
            cout << "  " << string(50, '=') << endl;

            //جست و جو بر اساس اسم دسته بازه محصول
            // اصلاح باگ: تنظیم صحیح متغیر found
            bool found = false;
            for (const auto& product : system->GetProduct()) {
                bool match = true;
                //شرط بررسی اسم محصول
                //npos = نشانه عدم یافتن زیر رشته
                if (!name.empty() && product.GetName().find(name) == string::npos) {
                    match = false;
                }
                //npos = نشانه عدم یافتن زیر رشته
                if (!category.empty() && product.GetCategory().find(category) == string::npos) {
                    match = false;
                }
                //npos = نشانه عدم یافتن زیر رشته
                if (product.GetPrice() < minPrice || product.GetPrice() > maxPrice) {
                    match = false;
                }
                if (match) {
                    product.display();
                    found = true;
                }
            }

            if (!found) {
                typeWriter("No products found with these criteria.", 50);
            }

            cout << "\nPress Enter to continue...";
            cin.get();
            break;
        }
        case 3: {
            showName("ACCOUNT CHARGE");
            double amount;
            cout << "  Current balance: " << balance << endl;
            cout << "  Enter amount to charge: ";
            // اصلاح باگ: مدیریت ورودی نامعتبر
            while (!(cin >> amount)) {
                typeWriter("Invalid input! Please enter a number.", 50);
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            chargeAccount(amount);
            break;
        }
        case 4: {
            showName("PRODUCT PURCHASE");
            system->displayProducts();
            string productName;
            cout << "\n  Enter product name to purchase: ";
            cin.ignore();
            getline(cin, productName);

            bool productFound = false;
            for (const auto& Product : system->GetProduct()) {
                if (Product.GetName() == productName) {
                    productFound = true;
                    if (makePurchase(Product.GetPrice())) {
                        addOrder(productName);
						system->saveUsers(); // ذخیره‌سازی فوری کاربران
                        cout << endl;
                        typeWriter("Purchase successful!", 45);
                        cout << "  Toman " << fixed << setprecision(2) << Product.GetPrice() << " deducted from your account." << endl;
                        cout.unsetf(ios_base::fixed); // اصلاح باگ: بازنشانی تنظیمات فرمت
                    }
                    else {
                        cout << endl;
                        typeWriter("Insufficient balance!", 45);
                    }
                    break;
                }
            }
            if (!productFound) {
                cout << endl;
                typeWriter("Product not found.", 45);
            }
            animateDelay(1800);
            break;
        }
        case 5: {
            showName("YOUR ORDERS");
            if (orders.empty()) {
                cout << endl;
                typeWriter("You have no orders yet.", 50);
            }
            else {
                cout << endl;
                for (const auto& order : orders) {
                    typeWriter("  • " + order, 30);
                }
            }
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        }
        case 6: {
            showName("ACCOUNT BALANCE");
            cout << endl;
            cout << "  Your current balance: Toman " << fixed << setprecision(2) << balance << endl;
            cout.unsetf(ios_base::fixed); // اصلاح باگ: بازنشانی تنظیمات فرمت
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        }
        case 0: {
            cout << endl;
            typeWriter("Logging out...", 50);
            animateDelay(1200);
            break;
        }
        default: {
            cout << endl;
            typeWriter("Invalid choice! Please try again.", 50);
            animateDelay(1200);
        }
        }
    } while (choice != 0);
}

void Seller::displayMenu() {
    // اصلاح باگ: مقداردهی system برای جلوگیری از کرش
    PetShopSystem* system = PetShopSystem::GetInstance();

    int choice;
    do {
        showName("SELLER DASHBOARD");
        cout << "  Welcome, " << username << "!\n" << endl;
        cout << "  1. Add Product" << endl;
        cout << "  2. View Products" << endl;
        cout << "  3. Remove Product" << endl;
        cout << "  4. View Customers" << endl;
        cout << "  0. Logout" << endl;
        cout << "\n  Enter your choice: ";
        // اصلاح باگ: مدیریت ورودی نامعتبر
        while (!(cin >> choice)) {
            typeWriter("Invalid input! Please enter a number.", 50);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
        case 1: {
            showName("ADD NEW PRODUCT");
            string name, category, suitablefor;
            double price;

            cout << "  Product name: ";
            cin.ignore();
            getline(cin, name);

            cout << "  Category: ";
            getline(cin, category);

            cout << "  Suitable for which pets: ";
            getline(cin, suitablefor);

            cout << "  Price: Toman ";
            // اصلاح باگ: مدیریت ورودی نامعتبر
            while (!(cin >> price)) {
                typeWriter("Invalid input! Please enter a number.", 50);
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            // اصلاح باگ: بررسی قیمت منفی
            if (price < 0) {
                typeWriter("Price cannot be negative!", 50);
                animateDelay(1200);
                break;
            }

            system->addProduct(Product(name, category, suitablefor, price));
            cout << endl;
            typeWriter("Product added successfully!", 45);
            animateDelay(1200);
            break;
        }
        case 2: {
            showName("PRODUCT LIST");
            system->displayProducts();
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        }
        case 3: {
            showName("REMOVE PRODUCT");
            system->displayProducts();
            string productName;
            cout << "\n  Enter product name to remove: ";
            cin.ignore();
            getline(cin, productName);
            system->removeProduct(productName);
            break;
        }
        case 4: {
            showName("CUSTOMER LIST");
            auto customers = system->getCustomers();
            if (customers.empty()) {
                cout << endl;
                typeWriter("No customers registered yet.", 50);
            }
            else {
                cout << endl;
                for (const auto& Customer : customers) {
                    typeWriter("  • " + Customer->GetUserName(), 30);
                }
            }
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        }
        case 0: {
            cout << endl;
            typeWriter("Logging out...", 50);
            animateDelay(1200);
            break;
        }
        default: {
            cout << endl;
            typeWriter("Invalid choice! Please try again.", 50);
            animateDelay(1200);
        }
        }
    } while (choice != 0);
}

int main() try {
    // اصلاح باگ: استفاده از Singleton به جای ایجاد مستقیم شیء
    PetShopSystem* PetShop = PetShopSystem::GetInstance();

    int MainChoice;

    do {
        showName("PET SHOP MANAGEMENT SYSTEM");
        cout << "  1. Login" << endl;
        cout << "  2. Register" << endl;
        cout << "  0. Exit" << endl;
        cout << "\n  Enter your choice: ";
        // اصلاح باگ: مدیریت ورودی نامعتبر
        while (!(cin >> MainChoice)) {
            typeWriter("Invalid input! Please enter a number.", 50);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (MainChoice) {
        case 1: {
            User* user = PetShop->Login();
            if (user) {
                user->displayMenu();
            }
            break;
        }
        case 2: {
            PetShop->registerUser();
            break;
        }
        case 0: {
            showName("THANK YOU FOR USING PET SHOP SYSTEM");
            typeWriter("Goodbye!", 120);
            animateDelay(1200);
            break;
        }
        default:
            cout << endl;
            typeWriter("Invalid choice! Please try again.", 50);
            animateDelay(1200);
            break;
        }
    } while (MainChoice != 0);

    return 0;
}
catch (const exception& error) {
    cerr << "Error: " << error.what() << endl;
    return 1;
}
