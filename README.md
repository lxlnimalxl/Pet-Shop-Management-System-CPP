# 🐾 Pet Shop Management System (C++)

A full-featured **Pet Shop Management System** implemented in C++ using **OOP (Object-Oriented Programming)** concepts.
This console-based application supports customer and seller roles, account management, product catalog, purchasing, and persistent data storage.

---

## 📌 Features

### ✅ General

* Text-based UI with **typewriter animation**
* Command line **screen clearing** for better UX
* Simple **Singleton pattern** for system management
* Cross-platform support (`cls`/`clear`)

### 👥 Users

* **User registration and login**
* Two roles: `Customer` and `Seller`
* Password-protected access with role-based dashboards

### 💼 Customers

* View product catalog
* Search for products (by name, category, price range)
* Charge account balance
* Purchase products (with balance deduction)
* View personal orders and remaining balance

### 🛙️ Sellers

* Add new products (with validation)
* View and remove products
* See registered customers

### 💾 Persistent Storage

* Data saved/loaded from `.txt` files:

  * `customers.txt`
  * `sellers.txt`
  * `products.txt`
* Handles comma-separated values with custom escaping/unescaping
* Data automatically loaded at startup and saved on exit

---

## 💠 Technologies Used

* **C++ Standard Library**

  * `iostream`, `fstream`, `vector`, `string`, `iomanip`, `map`, `limits`, `algorithm`, `sstream`
* **Threading & Animation**

  * `chrono` and `thread` for visual delays
* Object-Oriented principles:

  * **Encapsulation**, **Inheritance**, **Polymorphism**, **Abstract Classes**, **Virtual Functions**
* **Error handling** and **Input validation**

---

## 💂 Code Structure

* `User` (abstract base class)

  * `Customer`
  * `Seller`
* `Product`
* `PetShopSystem` (Singleton)
* Utilities:

  * `typeWriter()` – animated text output
  * `clearCmd()` – cross-platform screen clear
  * `escapeCommas()` – for file-safe string handling

---

## 🚀 How to Run

1. Make sure you have a C++ compiler (e.g., `g++`)
2. Compile the file:

   ```bash
   g++ -std=c++11 -o petshop main.cpp
   ```
3. Run:

   ```bash
   ./petshop
   ```

---

## 📂 Sample File Formats

### `customers.txt`

```
john123 pass123 500.00 toy,collar
```

### `sellers.txt`

```
admin1 adminpass
```

### `products.txt`

```
Dog Food,Food,Dogs,120.50
```

---

## 📌 Notes

* Product search supports **partial matches** for names and categories.
* Invalid input is handled with clear error messages.
* Commas in product names/categories are replaced with semicolons for file safety.
* Orders are saved per customer and loaded at startup.

---

## 🔐 Future Improvements

* Password encryption
* Admin panel
* Graphical UI (Qt / SDL)
* Product sorting and filtering
* More pet types and categories

---

## 📧 Author

Designed and developed by **\[Seyyed Nima Rounaghi]**
If you use or modify this project, consider giving it a ⭐ on GitHub!
