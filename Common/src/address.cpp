#include "common/address.h"

Address::Address()
{
    Street = "";
    City = "";
    Country = "";
    PostalCode = "";
}

Address::Address(string street, string city, string country, string postalcode)
{
    Street = street;
    City = city;
    Country = country;
    PostalCode = postalcode;
}

Address::Address(const Address& other)
{
    Street = other.Street;
    City = other.City;
    Country = other.Country;
    PostalCode = other.PostalCode;
}

Address& Address::operator=(const Address& other)
{
    Street = other.Street;
    City = other.City;
    Country = other.Country;
    PostalCode = other.PostalCode;
    return *this;
}

string Address::toString(){
    string result = "";
    result += "Street: " + Street + "\n";
    result += "City: " + City + "\n";
    result += "Country: " + Country + "\n";
    result += "PostalCode: " + PostalCode + "\n";
    return result; 
}