#include "common/userProfile.h"

UserProfile::UserProfile()
{
    Name = "";
    Age = 0;
    UserAddress = Address();
    Id = 0;
}

UserProfile::UserProfile(string name, int age, const Address& OtherUserAddress, int id)
{
    Name = name;
    Age = age;
    UserAddress = OtherUserAddress;
    Id = id;
}

UserProfile::UserProfile(string name, int age, string Street, string City, string Country, string PostalCode, int id)
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

int UserProfile::BirthDay()
{
    Age += 1;
    return Age;
}