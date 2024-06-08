#include <iostream>
#include <string>

using namespace std;

class BTreeNode {
public:
    int data;
    BTreeNode* left;
    BTreeNode* right;
};

class BTree {
private:
    BTreeNode* root;

public:
    BTree() {
        root = nullptr;
    }

    void insert(int key) {
        root = insertRec(root, key);
    }

    BTreeNode* insertRec(BTreeNode* root, int key) {
        if (root == nullptr) {
            BTreeNode* newNode = new BTreeNode();
            newNode->data = key;
            newNode->left = nullptr;
            newNode->right = nullptr;
            return newNode;
        }

        if (key < root->data)
            root->left = insertRec(root->left, key);
        else if (key > root->data)
            root->right = insertRec(root->right, key);

        return root;
    }

    BTreeNode* search(BTreeNode* root, int key) {
        if (root == nullptr || root->data == key)
            return root;

        if (root->data < key)
            return search(root->right, key);

        return search(root->left, key);
    }

    BTreeNode* getRoot() {
        return root;
    }
};

struct Value {
    string type;
    string value;
};

class Vector {
private:
    int* array;
    int capacity;
    int size;

public:
    Vector(int initialCapacity = 10) {
        capacity = initialCapacity;
        array = new int[capacity];
        size = 0;
    }

    ~Vector() {
        delete[] array;
    }

    void push_back(int value) {
        if (size == capacity) {
            capacity *= 2;
            int* newArray = new int[capacity];
            for (int i = 0; i < size; i++) {
                newArray[i] = array[i];
            }
            delete[] array;
            array = newArray;
        }
        array[size++] = value;
    }

    int& operator[](int index) {
        return array[index];
    }

    int getSize() const {
        return size;
    }
};

class Database {
private:
    struct TableColumn {
        string name;
        string type;
    };

    struct Record {
        Vector values;
    };

    BTree* tables[100];
    int tableCount;

    int hashData(const string& data) {
        int hashValue = 0;
        for (char c : data) {
            hashValue += static_cast<int>(c);
        }
        return hashValue;
    }

public:
    Database() {
        tableCount = 0;
    }

    void createTable(const string& tableName, const Vector& columns) {
        BTree* table = new BTree();
        tables[tableCount++] = table;

        table->insert(hashData(tableName)); // هش کردن نام جدول

        for (int i = 0; i < columns.getSize(); i++) {
            table->insert(hashData(to_string(i)));
        }
    }

    void insertRecord(const string& tableName, Vector& values) {
        BTree* table = getTable(tableName);
        if (table == nullptr) {
            
            return;
        }

        
        table->insert(hashData("Record"));

        
        BTreeNode* record = table->search(table->getRoot(), hashData("Record"));
        for (int i = 0; i < values.getSize(); i++) {
            record->left = new BTreeNode();
            record->left->data = values[i];
            record->left->left = nullptr;
            record->left->right = nullptr;
            record = record->left;
        }
    }

    void deleteRecord(const string& tableName, const string& condition) {
        BTree* table = getTable(tableName);
        if (table == nullptr) {
              return;
        }


        deleteRecords(table->getRoot(), condition);
    }

    void updateRecord(const string& tableName, const string& values, const string& condition) {
        BTree* table = getTable(tableName);
        if (table == nullptr) {
            
            return;
        }

        
        updateFields(table->getRoot(), values, condition);
    }

    void selectRecords(const string& tableName, const string& fields, const string& condition) {
        BTree* table = getTable(tableName);
        if (table == nullptr) {
            
            return;
        }

        selectFields(table->getRoot(), fields, condition);
    }

    void execute(const string& query) {
        string operation;
        string tableName;
        string fields;
        string condition;
        size_t pos;

        pos = query.find(' ');
        operation = query.substr(0, pos);

        if (operation == "CREATE") {
            pos = query.find(' ', pos + 1);
            tableName = query.substr(pos + 1);

            pos = query.find('(', pos + 1);
            fields = query.substr(pos + 1, query.length() - pos - 2);

            Vector columns = parseFields(fields);
            createTable(tableName, columns);
        } else if (operation == "INSERT") {
            pos = query.find(' ', pos + 1);
            pos = query.find(' ', pos + 1);
            tableName = query.substr(pos + 1);

            pos = query.find('(', pos + 1);
            fields = query.substr(pos + 1, query.length() - pos - 2);

            Vector values = parseFields(fields);
            insertRecord(tableName, values);
        } else if (operation == "DELETE") {
            pos = query.find(' ', pos + 1);
            pos = query.find(' ', pos + 1);
            tableName = query.substr(pos + 1);

            pos = query.find("WHERE", pos + 1);
            condition = query.substr(pos + 5);

            deleteRecord(tableName, condition);
        } else if (operation == "UPDATE") {
            pos = query.find(' ', pos + 1);
            tableName = query.substr(pos + 1);

            pos = query.find("SET", pos + 1);
            fields = query.substr(pos + 3, query.find("WHERE") - pos - 4);

            pos = query.find("WHERE", pos + 1);
            condition = query.substr(pos + 5);

            updateRecord(tableName, fields, condition);
        } else if (operation == "SELECT") {
            pos = query.find(' ', pos + 1);
            fields = query.substr(pos + 1, query.find("FROM") - pos - 2);

            pos = query.find("FROM", pos + 1);
            tableName = query.substr(pos + 5);

            pos = query.find("WHERE", pos + 1);
            condition = query.substr(pos + 5);

            selectRecords(tableName, fields, condition);
        }
    }

private:
    int getValueAsInt(const string& value) {
        string trimmedValue = trim(value);
        if (isInteger(trimmedValue)) {
            return stoi(trimmedValue);
        } else {
            throw invalid_argument(value);
        }
    }

    bool isInteger(const string& value) {
        if (value.empty() || ((!isdigit(value[0])) && (value[0] != '-') && (value[0] != '+'))) {
            return false;
        }

        int i = 1;
        while (i < value.length()) {
            if (!isdigit(value[i])) {
                return false;
            }
            i++;
        }

        return true;
    }

    string trim(const string& str) {
        size_t start = str.find_first_not_of(" ");
        size_t end = str.find_last_not_of(" ");

        if (start == string::npos)
            return "";

        return str.substr(start, end - start + 1);
    }

    bool evaluateCondition(BTreeNode* record, const string& condition) {
        int fieldValue = record->data;

        if (condition.find("==") != string::npos) {
            int targetValue = stoi(condition.substr(condition.find("==") + 2));
            return (fieldValue == targetValue);
        } else if (condition.find(">") != string::npos) {
            int targetValue = stoi(condition.substr(condition.find(">") + 1));
            return (fieldValue > targetValue);
        } else if (condition.find("<") != string::npos) {
            int targetValue = stoi(condition.substr(condition.find("<") + 1));
            return (fieldValue < targetValue);
        }

        return false;
    }

    void deleteRecords(BTreeNode* root, const string& condition) {
        if (root == nullptr)
            return;

        deleteRecords(root->left, condition);

        if (root->left != nullptr && evaluateCondition(root->left, condition)) {
            deleteTree(root->left);
            root->left = nullptr;
        }

        deleteRecords(root->right, condition);

        if (root->right != nullptr && evaluateCondition(root->right, condition)) {
            deleteTree(root->right);
            root->right = nullptr;
        }
    }

    void deleteTree(BTreeNode* root) {
        if (root == nullptr)
            return;

        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }

    void updateFields(BTreeNode* root, const string& values, const string& condition) {
        if (root == nullptr)
            return;

        updateFields(root->left, values, condition);

        if (root->left != nullptr && evaluateCondition(root->left, condition)) {
            BTreeNode* currentField = root->left;
            int index = 0;
            Vector fieldValues = parseFields(values);

            while (currentField != nullptr && index < fieldValues.getSize()) {
                currentField->data = fieldValues[index];
                currentField = currentField->left;
                index++;
            }
        }

        updateFields(root->right, values, condition);

        if (root->right != nullptr && evaluateCondition(root->right, condition)) {
            BTreeNode* currentField = root->right;
            int index = 0;
            Vector fieldValues = parseFields(values);

            while (currentField != nullptr && index < fieldValues.getSize()) {
                currentField->data = fieldValues[index];
                currentField = currentField->left;
                index++;
            }
        }
    }

    Vector parseFields(const string& fields) {
        Vector fieldValues;

        size_t pos = 0;
        while (pos < fields.length()) {
            size_t commaPos = fields.find(',', pos);
            string fieldValue = fields.substr(pos, commaPos - pos);

            fieldValues.push_back(getValueAsInt(fieldValue));

            pos = commaPos + 1;
        }

        return fieldValues;
    }

    void selectFields(BTreeNode* root, const string& fields, const string& condition) {
        if (root == nullptr)
            return;

        selectFields(root->left, fields, condition);

        if (evaluateCondition(root, condition)) {
            Vector fieldNames = parseFields(fields);

            for (int i = 0; i < fieldNames.getSize(); i++) {
                BTreeNode* currentField = root->left;
                int index = 0;
                while (currentField != nullptr && index < fieldNames[i]) {
                    currentField = currentField->left;
                    index++;
                }

                if (currentField != nullptr)
                    cout << currentField->data << "\t";
            }
            cout << endl;
        }

        selectFields(root->right, fields, condition);
    }

    BTree* getTable(const string& tableName) {
        int hashedTableName = hashData(tableName); 
        for (int i = 0; i < tableCount; i++) {
            BTree* table = tables[i];
            if (hashedTableName == table->getRoot()->data) { 
            }
        }
        return nullptr;
    }
};

int main() {
    Database database;
    string query;
    int numQueries;
    cin >> numQueries;
    cin.ignore();

    for (int i = 0; i < numQueries; i++) {
      
        getline(cin, query);} 
        
        database.execute(query);


    return 0;
}
