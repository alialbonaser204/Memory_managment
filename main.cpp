#include <iostream>
#include <cassert>
#include <cstring>

struct Hm_item {
    const char* key;
    const char* value;
    Hm_item* next;

    // Ruimt dit item op: verwijdert de key-string, de value-string,
    // en roept via 'next' ook de destructor van het volgende item aan,
    // zodat de hele keten wordt opgeruimd.
    ~Hm_item(){
        delete[] key;
        delete[] value;
        delete next;
    }
};

struct HashMap
{
    Hm_item** items;
    unsigned int size;
    unsigned int count;

    // Ruimt elke lade van de array op (via delete items[i], wat de hele
    // rij kaartjes in die lade opruimt), en ruimt daarna de array zelf op.
    ~HashMap(){
        for (unsigned int i = 0; i < size; i++)
        {
            delete items[i];
        }
        delete[] items;
    }
};

// Telt de ASCII-waarde van elke letter in de string bij elkaar op,
// en gebruikt daarna de rest bij deling door 'size' (modulo), zodat
// de uitkomst altijd een geldig vakje-nummer is (tussen 0 en size-1).
unsigned long hash(const char* str, const unsigned int& size)
{
    unsigned long total = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        total += str[i];
    }
    return total % size;
}

// Maakt een nieuwe, lege HashMap: reserveert de array met 'size' laden,
// en zet elke lade op nullptr omdat er nog niks in zit.
HashMap* createHashMap(const unsigned int& size)
{
    HashMap* hm = new HashMap();
    hm->size = size;
    hm->count = 0;
    hm->items = new Hm_item*[size];
    for (unsigned int i = 0; i < size; i++)
    {
        hm->items[i] = nullptr;
    }
    return hm;
}

// Maakt één los kaartje aan met de gegeven key en value.
// 'next' staat nog op nullptr, want dit kaartje hangt nog nergens in.
Hm_item* createItem(const char* key, const char* value)
{
    Hm_item* item = new Hm_item();
    item->key = key;
    item->value = value;
    item->next = nullptr;
    return item;
}

// Maakt een nieuw kaartje aan, berekent met hash() in welke lade het moet,
// en hangt het vooraan in de rij kaartjes van die lade (zo blijven meerdere
// kaartjes met dezelfde lade-index, bij collisions, gewoon bewaard).
void addItem(HashMap* hm, const char* key, const char* value)
{
    Hm_item* item = createItem(key, value);
    unsigned long index = hash(key, hm->size);
    item->next = hm->items[index];
    hm->items[index] = item;
    hm->count++;
}

// Zoekt met hash() de juiste lade op, en loopt daarna de rij kaartjes
// van die lade af tot de key gevonden is. Is de key niet aanwezig,
// dan wordt nullptr teruggegeven.
Hm_item* findItem(const HashMap* hm, const char* key)
{
    unsigned long index = hash(key, hm->size);
    Hm_item* item = hm->items[index];
    while (item != nullptr)
    {
        if (std::strcmp(item->key, key) == 0)
        {
            return item;
        }
        item = item->next;
    }
    return nullptr;
}

// Verwijdert de hele HashMap. delete hm roept automatisch de destructor
// van HashMap aan, die op zijn beurt elke lade en de array zelf opruimt.
void deleteHashMap(HashMap* hm)
{
    delete hm;
}

/// @brief Tests if the created hash is consistent and not too large
/// @param size The max size of the hash
void testHash(const unsigned int& size)
{
    unsigned int hashedValue = hash("apple", size);
    assert(hashedValue == hash("apple", size));
    assert(hashedValue < size);
}

/// @brief Tests if a hash map is created and deleted without problems,
/// and whether all members are set correctly
/// @param size The size of the hash map
void testCreateAndDeleteHashMap(const unsigned int& size)
{
    HashMap* hm = createHashMap(size);

    assert(hm->size == size);
    assert(hm->count == 0);
    for (int i = 0; i < hm->size; i++)
    {
        assert(hm->items[i] == nullptr);
    }

    deleteHashMap(hm);
}

/// @brief Tests if items are added to the hash map correctly
/// @param size The size of the hash map
void testAddItems(const unsigned int& size)
{
    HashMap* hm = createHashMap(size);
    const char* key1 = new char[6]{'a', 'p', 'p', 'l', 'e', '\0'};
    const char* value1 = new char[6]{'f', 'r', 'u', 'i', 't', '\0'};
    const char* key2 = new char[7]{'b', 'a', 'n', 'a', 'n', 'a', '\0'};
    const char* value2 = new char[7]{'y', 'e', 'l', 'l', 'o', 'w', '\0'};
    addItem(hm, key1, value1);
    addItem(hm, key2, value2);

    assert(hm->count == 2);
    assert(std::strcmp(findItem(hm, key1)->key, "apple") == 0);
    assert(std::strcmp(findItem(hm, key1)->value, "fruit") == 0);
    assert(std::strcmp(findItem(hm, key2)->key, "banana") == 0);
    assert(std::strcmp(findItem(hm, key2)->value, "yellow") == 0);

    deleteHashMap(hm);
}

/// @brief Tests added items can be found correctly
/// @param size The size of the hash map
void testFindItems(const unsigned int& size)
{
    HashMap* hm = createHashMap(size);
    const char* key1 = new char[6]{'a', 'p', 'p', 'l', 'e', '\0'};
    const char* value1 = new char[6]{'f', 'r', 'u', 'i', 't', '\0'};
    const char* key2 = new char[7]{'b', 'a', 'n', 'a', 'n', 'a', '\0'};
    const char* value2 = new char[7]{'y', 'e', 'l', 'l', 'o', 'w', '\0'};
    addItem(hm, key1, value1);
    addItem(hm, key2, value2);

    assert(std::strcmp(findItem(hm, key1)->key, "apple") == 0);
    assert(std::strcmp(findItem(hm, key1)->value, "fruit") == 0);

    deleteHashMap(hm);
}

/// @brief Tests whether the hashmap handles collisions correctly,
/// by creating a hashmap of size 1 (so collisions always occur)
void testCollision()
{
    HashMap* hm = createHashMap(1);
    const char* key1 = new char[6]{'a', 'p', 'p', 'l', 'e', '\0'};
    const char* value1 = new char[6]{'f', 'r', 'u', 'i', 't', '\0'};
    const char* key2 = new char[7]{'b', 'a', 'n', 'a', 'n', 'a', '\0'};
    const char* value2 = new char[7]{'y', 'e', 'l', 'l', 'o', 'w', '\0'};
    addItem(hm, key1, value1);
    addItem(hm, key2, value2);

    assert(std::strcmp(findItem(hm, key1)->key, "apple") == 0);
    assert(std::strcmp(findItem(hm, key1)->value, "fruit") == 0);
    assert(std::strcmp(findItem(hm, key2)->key, "banana") == 0);
    assert(std::strcmp(findItem(hm, key2)->value, "yellow") == 0);

    deleteHashMap(hm);
}

int main()
{
    const unsigned int size = 5000;
    testHash(size);
    std::cout << "The test 'testHash' has succeeded!" << std::endl;
    testCreateAndDeleteHashMap(size);
    std::cout << "The test 'testCreateAndDeleteHashMap' has succeeded!" << std::endl;
    testAddItems(size);
    std::cout << "The test 'testAddItems' has succeeded!" << std::endl;
    testFindItems(size);
    std::cout << "The test 'testFindItems' has succeeded!" << std::endl;
    testCollision();
    std::cout << "The test 'testCollision' has succeeded!" << std::endl;

    return 0;
}
