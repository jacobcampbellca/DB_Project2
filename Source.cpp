/* Standard C++ includes */
//#include <cstdlib>
#include <iostream>
#include <string>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <conio.h>


using namespace std;

// reads in a password without echoing it to the screen
//    WARNING: Does not work in VS2015. 
string myget_passwd()
}
{
	string passwd;

	for (;;)
	{
		char ch;
		ch = _getch();           // get char WITHIOUT echo!
		if (ch == 13 || ch == 10) // if done ...
			break;           //  stop reading chars!
		cout << '*';  // dump * to screen
		passwd += ch;   // addd char to password
	}
	cin.sync(); // flush anything else in the buffer (remaining newline)
	string dummy;
	getline(cin, dummy);
	cout << endl;  // simulate the enter that the user pressed

	return passwd;
}



int main(void)
{
	string db_host, db_user, db_password, db_name;
	cout << "Enter hostname: ";
	cin >> db_host;

	cout << "Enter username: ";
	cin >> db_user;

	cout << "Enter password: ";
	db_password = myget_passwd();
	cout << endl;

	db_name = db_user;


	try {
		sql::Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		sql::ResultSet* res;

		// Create a connection */
		driver = get_driver_instance();
		con = driver->connect(db_host, db_user, db_password);

		// choose database
		con->setSchema(db_name);

		if (con == NULL)
		{
			cout << "Connection Failed!!" << endl;
			return 1;
		}

		//create cars table if one does not already exist
		string myQuery = "create table if not exists car ";
		myQuery += "(VIN char(17), miles int,  dealer varchar(40), price int,";
		myQuery += "PRIMARY KEY(VIN), FOREIGN KEY(dealer) references dealership(dealerName))";
		stmt = con->createStatement();
		stmt->execute(myQuery);
		delete stmt;

		//create manufacturers table if one does not already exist
		myQuery = "create table if not exists manufacturer ";
		myQuery += "(manufacturerAbv char(3), manufacturerName varchar(40),";
		myQuery += "PRIMARY KEY(manufacturerAbv) )";
		stmt = con->createStatement();
		stmt->execute(myQuery);
		delete stmt;

		//create dealerships table if one does not already exist
		myQuery = "create table if not exists dealership ";
		myQuery += "(dealerName varchar(40), zip int, phoneNumber char(10), ";
		myQuery += "PRIMARY KEY(dealerName, zip) )";
		stmt = con->createStatement();
		stmt->execute(myQuery);
		delete stmt;


		//initialize variables
		char primaryCommand = NULL;
		char secondaryCommand = NULL;
		string VIN, checkVIN, thisVIN, miles, dealer, price;
		string manufacturerAbv, checkManufacturer, abv, manufacturerName;
		string dealerName, name, phoneNumber, zip, checkDealerName, thisName, thisNumber, checkDealer;
		string getCars, getDealers, getManufacturers;
		string findManufacturer, findZIP;
		string deleteCar, deleteDealer;
		string summarizeManufacturers;
		bool manufacturerExists, dealerExists, duplicate;


		//while use does not want to quit the program...
		while (primaryCommand != 'q' || primaryCommand != 'Q') {
			cout << ">>> ";
			cin >> primaryCommand;

			//if user does want to quit the program, break from the loop
			if (primaryCommand == 'q' || primaryCommand == 'Q') {
				break;
			}

			//give average price of car made by each manufacturer
			//skip to the end of the loop
			else if (primaryCommand == 's' || primaryCommand == 'S') {
				summarizeManufacturers = "select manufacturerName, AVG(price) as avgPrice from manufacturer, car ";
				summarizeManufacturers += "where substring(VIN, 1, 3) = manufacturerAbv group by manufacturerName ";
				summarizeManufacturers += "order by avgPrice asc";
				stmt = con->createStatement();
				res = stmt->executeQuery(summarizeManufacturers);
				while (res->next()) {
					cout << res->getString("manufacturerName") << " ";
					cout << res->getString("avgPrice") << endl;
				}
				delete stmt;
				delete res;
				goto here;
			}

			cin >> secondaryCommand;

			//add some value to tables
			if (primaryCommand == 'a' || primaryCommand == 'A') {

				//add car to car table
				if (secondaryCommand == 'c' || secondaryCommand == 'C') {

					//get user input for car
					cin >> VIN >> miles >> dealer >> price;

					//my sql command to insert user values into car table
					myQuery = "insert into car values (\"";
					myQuery += VIN;
					myQuery += "\", ";
					myQuery += miles;
					myQuery += ", \"";
					myQuery += dealer;
					myQuery += "\", ";
					myQuery += price;
					myQuery += ")";

					//check that car manufacturer exists in manufacturer table
					checkManufacturer = "select manufacturerAbv from manufacturer";
					stmt = con->createStatement();
					res = stmt->executeQuery(checkManufacturer);
					manufacturerExists = false;
					while (res->next()) {
						abv = res->getString("manufacturerAbv");
						if (abv == VIN.substr(0, 3)) {
							manufacturerExists = true;
							break;
						}
					}

					//if manufacturere does not exist, print error
					if (!manufacturerExists) {
						cout << "ERROR: manufacturer does not exist" << endl;
					}

					//check that dealer exists in dealership table
					checkDealer = "select dealerName from dealership";
					res = stmt->executeQuery(checkDealer);
					dealerExists = false;
					while (res->next()) {
						name = res->getString("dealerName");
						if (name == dealer) {
							dealerExists = true;
							break;
						}
					}

					//if dealer does not exist, print error
					if (!dealerExists) {
						cout << "ERROR: dealer does not exist" << endl;
					}

					//check for suplicate VIN
					checkVIN = "select VIN from car";
					res = stmt->executeQuery(checkVIN);
					duplicate = false;
					while (res->next()) {
						thisVIN = res->getString("VIN");
						if (thisVIN == VIN) {
							cout << "ERROR: duplicate VIN" << endl;
							duplicate = true;
							break;
						}
					}

					//if the car manufacturer exists, and the VIN is not already in the table
					//execute command to add car to car table
					if (manufacturerExists && dealerExists && !duplicate) {
						stmt->execute(myQuery);
					}
					delete stmt;
					delete res;

				}

				//add manufacturer to manufacturer table
				else if (secondaryCommand == 'm' || secondaryCommand == 'M') {

					//get user input
					cin >> manufacturerAbv >> manufacturerName;

					//mysql command to add manufacturer into manufacturere table
					myQuery = "insert into manufacturer values (\"";
					myQuery += manufacturerAbv;
					myQuery += "\", \"";
					myQuery += manufacturerName;
					myQuery += "\")";

					//check to see if duplicate manufacturer
					checkManufacturer = "select manufacturerAbv from manufacturer";
					stmt = con->createStatement();
					res = stmt->executeQuery(checkManufacturer);
					duplicate = false;
					while (res->next()) {
						abv = res->getString("manufacturerAbv");
						if (abv == manufacturerAbv) {
							cout << "ERROR: duplicate manufacturer" << endl;
							duplicate = true;
						}
					}

					//if manufacturer is not already in the table
					//execute sql command to insert manufacturer into manufactuer table
					if (!duplicate) {
						stmt->execute(myQuery);
					}

					delete stmt;
					delete res;
				}

				//add dealership to dealership table
				else if (secondaryCommand == 'd' || secondaryCommand == 'D') {

					//get user inoput
					cin >> dealerName >> zip >> phoneNumber;

					//sql command to insert dealership into dealership table
					myQuery = "insert into dealership values (\"";
					myQuery += dealerName;
					myQuery += "\", ";
					myQuery += zip;
					myQuery += ", \"";
					myQuery += phoneNumber;
					myQuery += "\")";

					//check for duplicate dealership
					checkDealerName = "select dealerName from dealership";
					stmt = con->createStatement();
					res = stmt-> executeQuery(checkDealerName);
					duplicate = false;
					while (res->next()) {
						thisName = res->getString("dealerName");
						if (thisName == dealerName) {
							cout << "ERROR: duplicate dealership" << endl;
							duplicate = true;
						}
					}

					//if dealership is not duplicate, execute command to insert dealership into table
					if (!duplicate) {
						stmt->execute(myQuery);
					}
					delete stmt;
					delete res;
				}
			}

			//list command
			else if (primaryCommand == 'l' || primaryCommand == 'L') {
				
				//list cars in car table
				if (secondaryCommand == 'c' || secondaryCommand == 'C') {
					
					//get all columns from cars table
					getCars = "select * from car order by VIN";
					stmt = con->createStatement();
					res = stmt->executeQuery(getCars);
					
					//print all values in cars table to the screen
					while (res->next()) {
						cout << res->getString("VIN") << " ";
						cout << res->getString("miles") << " ";
						cout << res->getString("dealer") << " ";
						cout << res->getString("price") << endl;
					}
					delete stmt;
					delete res;
				}

				//list dealerships in dealership table
				else if (secondaryCommand == 'd' || secondaryCommand == 'D') {

					//get all columns from dealership table and order them in
					//ascending order based on ZIP code, and dealership name (if 
					//in same zip code)
					getDealers = "select * from dealership order by zip ASC, dealerName";
					stmt = con->createStatement();
					res = stmt->executeQuery(getDealers);

					//print all values in the dealership table to the screen
					while (res->next()) {
						cout << res->getString("dealerName") << " ";
						cout << res->getString("zip") << " ";
						thisNumber = res->getString("phoneNumber");
						cout << "(" << thisNumber.substr(0, 3) << ")";
						cout << thisNumber.substr(3, 3) << "-";
						cout << thisNumber.substr(6, 4) << endl;
					}
					delete stmt;
					delete res;
				}

				//list all manufacturers in manufacturer table
				else if (secondaryCommand == 'm' || secondaryCommand == 'M') {
					
					//get all columns from manufacturers table
					getManufacturers = "select * from manufacturer";
					stmt = con->createStatement();
					res = stmt->executeQuery(getManufacturers);

					//print all values in manufacturer table to the screen
					while (res->next()) {
						cout << res->getString("manufacturerAbv") << " ";
						cout << res->getString("manufacturerName") << endl;
					}
					delete stmt;
					delete res;
				}

			}

			//find command
			else if (primaryCommand == 'f' || primaryCommand == 'F') {

				//find all cars made by a specific manufacturer
				if (secondaryCommand == 'm' || secondaryCommand == 'M') {

					//determine desired manufacturer entered by user.
					//create a query to find all cars made by that manufacturer,
					//as well as their milleage, price, dealership, and dealership phone number
					cin >> manufacturerName;
					findManufacturer = "select miles, price, dealerName, phoneNumber";
					findManufacturer += " from car, dealership, manufacturer";
					findManufacturer += " where manufacturerName = \"";
					findManufacturer += manufacturerName;
					findManufacturer += "\" and manufacturerAbv = substring(VIN, 1, 3) ";
					findManufacturer += "and dealer = dealerName";
					findManufacturer += " order by price DESC, miles ASC, dealerName";
					stmt = con->createStatement();
					res = stmt->executeQuery(findManufacturer);

					//print all cars made by desired manufacturer to the screen
					while (res->next()) {
						cout << res->getString("miles") << " ";
						cout << "$" << res->getString("price") << " ";
						cout << res->getString("dealerName") << " ";
						thisNumber = res->getString("phoneNumber");
						cout << "(" << thisNumber.substr(0, 3) << ")";
						cout << thisNumber.substr(3, 3) << "-";
						cout << thisNumber.substr(6, 4) << endl;
					}
					delete stmt;
					delete res;
					
				}

				//find all cars made in a specific area code
				else if (secondaryCommand == 'z' || secondaryCommand == 'Z') {
					
					//determine desired zip code entered by user.
					//create a query to find all cars being sold in the zip code,
					//as well as their manufacturer, milegae, price, dealership, and dealership phone number
					cin >> zip;
					findZIP = "select manufacturerName, miles, price, dealerName, phoneNumber ";
					findZIP += "from car, manufacturer, dealership ";
					findZIP += "where zip = \"";
					findZIP += zip;
					findZIP += "\" and dealer = dealerName and manufacturerAbv = substring(VIN, 1, 3) ";
					findZIP += "order by manufacturerName, price DESC, dealerName";
					stmt = con->createStatement();
					res = stmt->executeQuery(findZIP);

					//print all car being sold in the desired zip code to the screen
					while (res->next()) {
						cout << res->getString("manufacturerName") << " ";
						cout << res->getString("miles") << " ";
						cout << "$" << res->getString("price") << " ";
						cout << res->getString("dealerName") << " ";
						thisNumber = res->getString("phoneNumber");
						cout << "(" << thisNumber.substr(0, 3) << ")";
						cout << thisNumber.substr(3, 3) << "-";
						cout << thisNumber.substr(6, 4) << endl;
					}
					delete stmt;
					delete res;
					
				}
			}

			//delete command
			else if (primaryCommand == 'd' || primaryCommand == 'D') {
				
				//delete car from car table
				if (secondaryCommand == 'c' || secondaryCommand == 'C') {
					
					//get VIN of car to be deleted.
					//delete car from car table where user input equal VIN
					cin >> VIN;
					deleteCar = "delete from car where VIN = \"" + VIN + "\"";
					stmt = con->createStatement();
					stmt->execute(deleteCar);
					delete stmt;
				}

				//delete dealer and cars sold by dealer
				else if (secondaryCommand == 'd' || secondaryCommand == 'D') {
					
					//get name of dealer to be deleted
					cin >> dealerName;

					//delete dealership from dealership table where user input is the name of the dealership
					deleteDealer = "delete from dealership where dealerName = \"" + dealerName + "\"";

					//delete car from car table where user input is the name of the car dealership
					deleteCar = "delete from car where dealer = \"" + dealerName + "\"";
					stmt = con->createStatement();
					stmt->execute(deleteDealer);
					stmt->execute(deleteCar);
					delete stmt;
				}
			}
			here:
			cout << "------------------------------------------" << endl;
		}
	}

	//catch error if database, user, or password are incorrect;
	//or if sequel command contains syntax error
	catch (sql::SQLException& e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	//exit the program
	cout << "BYE" << endl;
	return 0;
}