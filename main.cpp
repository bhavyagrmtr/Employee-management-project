#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;
using namespace sql;

void login(Connection* conn);
void addEmployee(Connection* conn);
void viewEmployees(Connection* conn);
bool validateUser(Connection* conn, string username, string password);

int main() {
    mysql::MySQL_Driver* driver;
    Connection* conn;

    driver = mysql::get_mysql_driver_instance();
    conn = driver->connect("tcp://127.0.0.1:3306", "root", "password");

    conn->setSchema("employee_management");

    login(conn);

    delete conn;
    return 0;
}

void login(Connection* conn) {
    string username, password;

    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (validateUser(conn, username, password)) {
        int choice;

        do {
            cout << "\n1. Add Employee\n2. View Employees\n3. Exit\nEnter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    addEmployee(conn);
                    break;
                case 2:
                    viewEmployees(conn);
                    break;
                case 3:
                    cout << "Exiting...\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 3);
    } else {
        cout << "Invalid login credentials. Please try again.\n";
    }
}

bool validateUser(Connection* conn, string username, string password) {
    PreparedStatement* pstmt;
    ResultSet* res;
    bool isValid = false;

    pstmt = conn->prepareStatement("SELECT * FROM users WHERE username = ? AND password = ?");
    pstmt->setString(1, username);
    pstmt->setString(2, password);
    res = pstmt->executeQuery();

    if (res->next()) {
        isValid = true;
    }

    delete res;
    delete pstmt;
    return isValid;
}

void addEmployee(Connection* conn) {
    string name, position, department;
    double salary;
    PreparedStatement* pstmt;

    cout << "Enter employee name: ";
    cin >> ws; // To consume any leading whitespace
    getline(cin, name);
    cout << "Enter position: ";
    getline(cin, position);
    cout << "Enter salary: ";
    cin >> salary;
    cout << "Enter department: ";
    cin >> ws;
    getline(cin, department);

    pstmt = conn->prepareStatement("INSERT INTO employees (name, position, salary, department) VALUES (?, ?, ?, ?)");
    pstmt->setString(1, name);
    pstmt->setString(2, position);
    pstmt->setDouble(3, salary);
    pstmt->setString(4, department);
    pstmt->execute();

    cout << "Employee added successfully.\n";

    delete pstmt;
}

void viewEmployees(Connection* conn) {
    Statement* stmt;
    ResultSet* res;

    stmt = conn->createStatement();
    res = stmt->executeQuery("SELECT * FROM employees");

    while (res->next()) {
        cout << "ID: " << res->getInt("id")
             << ", Name: " << res->getString("name")
             << ", Position: " << res->getString("position")
             << ", Salary: " << res->getDouble("salary")
             << ", Department: " << res->getString("department") << endl;
    }

    delete res;
    delete stmt;
}
