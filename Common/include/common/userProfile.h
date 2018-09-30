#include <iostream>
#include <string>
#include "common/address.h"

using namespace std;

class UserProfile
{
    public:
        string Name;
        int Age;
        Address UserAddress;
        int Id;

        UserProfile();
        UserProfile(string name, int age, const Address& UserAddress, int id);
        UserProfile(string name, int agege, string Street, string City, string Country, string PostalCode, int id);
        UserProfile(const UserProfile& other);

        int BirthDay();

        UserProfile& operator=(const UserProfile& other);

        friend ostream& operator<<(ostream& out, const UserProfile& data){
            out << "Id: " << data.Id << endl;
            out << "Name: " << data.Name.c_str() << endl;
            out << "Age: " << data.Age << endl;
            out << "Address: " << endl;
            out << data.UserAddress << endl;
            return out; 
        }

};