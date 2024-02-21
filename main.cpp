// Домашная работа номер 6 от 20.02.2024

#include <iostream>
#include <wt/dbo/dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <windows.h>

#pragma execution_character_set( "utf-8")

using namespace std;
using namespace Wt::Dbo;

class Book;
class Stock;
class Sale;


class Publisher 
{
public:
	string name;
	collection<ptr<Book>> books;

	template<typename Action>
	void persist(Action& a)
	{
		field(a, name, "name");
		hasMany(a, books, ManyToOne, "publisher");
	}
};

class Book
{
public:
	string title;
	ptr<Publisher> publisher;
	collection<ptr<Stock>> stocks;

	template<typename Action>
	void persist(Action& a)
	{
		field(a, title, "title");
		belongsTo(a, publisher, "publisher");
		hasMany(a, stocks, ManyToOne, "book");
	}
};

class Shop
{
public:
	string name;
	collection<ptr<Stock>> stocks;

	template<typename Action>
	void persist(Action& a)
	{
		field(a, name, "name");
		hasMany(a, stocks, ManyToOne, "shop");
	}
};

// Это стопка книг, которая отправляется в разные магазины
class Stock
{
public:
	int count;
	ptr<Book> books;
	ptr<Shop> shop;
	collection<ptr<Sale>> sales;

	template<typename Action>
	void persist(Action& a)
	{
		field(a, count, "count");
		belongsTo(a, books, "book");
		belongsTo(a, shop, "shop");
		hasMany(a, sales, ManyToOne, "stock");
	}
};

// мы из стопки продаём и продаём книги
class Sale
{
public:
	int count;
	int price;
	//YYYY-MM-DD - таком виде строки сортируются автоматически
	string date_sale;
	ptr<Stock> stocks;

	template<typename Action> 
	void persist(Action& a)
	{
		field(a, count, "count");
		field(a, price, "price");
		field(a, date_sale, "date_sale");
		belongsTo(a, stocks, "stock");
	}
};





int main()
{

	//setlocale(LC_ALL, "Russian");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1000);
	try
	{
		std::string connectionString =
			"host=127.0.0.1 "
			"port=5432 "
			"dbname=lesson06 "
			"user=postgres "
			"password=111111";

		std::unique_ptr<backend::Postgres> connection{ new backend::Postgres(connectionString) };

		Session session;
		session.setConnection(std::move(connection));
		//Регистрируем класс Publisher

		session.mapClass<Publisher>("publisher");
		session.mapClass<Shop>("shop");
		session.mapClass<Book>("book");
		session.mapClass<Stock>("stock");
		session.mapClass<Sale>("sale");

		try
		{
			session.createTables();
		}
		catch (...)
		{

		}

		

		Transaction tx(session);

	// Создаём три издательства - как объекты реальной жизни

		unique_ptr<Publisher> publisher1{ new Publisher{"ESKMO", {}} };
		unique_ptr<Publisher> publisher2{ new Publisher{"AST", {}} };
		unique_ptr<Publisher> publisher3{ new Publisher{"PITER", {}} };


		auto publisher1ptr = session.add(move(publisher1));
		auto publisher2ptr = session.add(move(publisher2));
		auto publisher3ptr = session.add(move(publisher3));


		// Добавляем магазины

		unique_ptr<Shop> shop1{ new Shop{ "Na Moike", {}} };
		unique_ptr<Shop> shop2{ new Shop{ "Nevskii", {}} };
		unique_ptr<Shop> shop3{ new Shop{ "Prosveshenia", {}} };
		unique_ptr<Shop> shop4{ new Shop{ "Sennaya", {}} };

		auto shopNaMoike = session.add(move(shop1));
		auto shopNevskii = session.add(move(shop2));
		auto shopProsveshenia = session.add(move(shop3));
		auto shopSennaya = session.add(move(shop4));

		// добавляем книги

		unique_ptr<Book> book1{ new Book{ " My ", {}, {} } };
		unique_ptr<Book> book2{ new Book{ " We ", {}, {} } };
		unique_ptr<Book> book3{ new Book{ " They ", {}, {} } };

		auto book1ptr = session.add(move(book1));
		auto book2ptr = session.add(move(book2));
		auto book3ptr = session.add(move(book3));

		// Определяем, к какому издателю какие книги относятся
		book1ptr.modify()->publisher = publisher2ptr;
		book2ptr.modify()->publisher = publisher3ptr;
		book3ptr.modify()->publisher = publisher1ptr;

		// Заполняем таблицу для стопок (stocks)

		unique_ptr<Stock> stock1{ new Stock{ 100, {}, {} } };
		unique_ptr<Stock> stock2{ new Stock{ 100, {}, {} } };
		unique_ptr<Stock> stock3{ new Stock{ 100, {}, {} } };
		unique_ptr<Stock> stock4{ new Stock{ 100, {}, {} } };
		unique_ptr<Stock> stock5{ new Stock{ 100, {}, {} } };

		auto stock1ptr = session.add(move(stock1));
		auto stock2ptr = session.add(move(stock2));
		auto stock3ptr = session.add(move(stock3));
		auto stock4ptr = session.add(move(stock4));
		auto stock5ptr = session.add(move(stock5));

		stock1ptr.modify()->books = book1ptr;
		stock1ptr.modify()->shop = shopNaMoike;

		stock2ptr.modify()->books = book3ptr;
		stock2ptr.modify()->shop = shopProsveshenia;

		stock3ptr.modify()->books = book2ptr;
		stock3ptr.modify()->shop = shopProsveshenia;

		stock4ptr.modify()->books = book1ptr;
		stock4ptr.modify()->shop = shopSennaya;

		stock4ptr.modify()->books = book2ptr;
		stock4ptr.modify()->shop = shopNaMoike;

		// продажи

		unique_ptr<Sale> sale1{ new Sale{ 10, 200, "2023-11-09", {} } };
		unique_ptr<Sale> sale2{ new Sale{ 50, 150, "2023-11-19", {} } };
		unique_ptr<Sale> sale3{ new Sale{ 1, 350, "2023-11-08", {} } };
		unique_ptr<Sale> sale4{ new Sale{ 3, 580, "2023-11-10", {} } };
		unique_ptr<Sale> sale5{ new Sale{ 15, 150, "2023-11-09", {} } };

		auto sale1ptr = session.add(move(sale1));
		auto sale2ptr = session.add(move(sale2));
		auto sale3ptr = session.add(move(sale3));
		auto sale4ptr = session.add(move(sale4));
		auto sale5ptr = session.add(move(sale5));

		sale1ptr.modify()->stocks = stock1ptr;
		sale2ptr.modify()->stocks = stock1ptr;
		sale3ptr.modify()->stocks = stock2ptr;
		sale4ptr.modify()->stocks = stock2ptr;
		sale5ptr.modify()->stocks = stock3ptr;

		tx.commit();
	}

	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

}