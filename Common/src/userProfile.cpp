#include "common/userProfile.h"

UserProfile::UserProfile()
{
    Name = "";
    Age = 0;
    UserAddress = Address();
    Id = 0;
}

UserProfile::UserProfile(string name, int age, const Address& OtherUserAddress, size_t id)
{
    Name = name;
    Age = age;
    UserAddress = OtherUserAddress;
    Id = id;
}

UserProfile::UserProfile(string name, int age, string Street, string City, string Country, string PostalCode, size_t id)
{
    Name = name;
    Age = age;
    UserAddress = Address(Street, City, Country, PostalCode);
    Id = id;
}

UserProfile::UserProfile(const UserProfile& other)
{
    Name = other.Name;
    Age = other.Age;
    UserAddress = other.UserAddress;
    Id = other.Id;
}

UserProfile& UserProfile::operator=(const UserProfile& other)
{
    Name = other.Name;
    Age = other.Age;
    UserAddress = other.UserAddress;
    Id = other.Id;
    return *this;
}

string UserProfile::BirthDay()
{
    Age += 1;
    string response = Name + " had a birthday, he is now " + to_string(Age) + " years old";
    return response;
}

string UserProfile::toString(){
    string result = "";
    result += "Id: " + to_string(Id) + "\n";
    result += "Name: " + Name + "\n";
    result += "Age: " + to_string(Age) + "\n";
    result += "Address: " + UserAddress.toString() + "\n";
    return result; 
}
