CREATE DATABASE SALEMANAGER
GO

USE SALEMANAGER
GO

CREATE TABLE CUSTOMER (
	ID INT,
	NAME VARCHAR(50),
	CITY VARCHAR (50)
	
	CONSTRAINT PK_CUSTOMER
	PRIMARY KEY (ID)
)

CREATE TABLE ORDERING (
	O_ID VARCHAR(10),
	DATE VARCHAR(20),
	CUSTOMER_ID INT,
	AMOUNT INT
	
	CONSTRAINT PK_ORDER
	PRIMARY KEY (O_ID)
)

CREATE TABLE PRODUCT (
	PID VARCHAR(10),
	PNAME VARCHAR(50),
	TYPE VARCHAR (50)

	CONSTRAINT PK_PRODUCT
	PRIMARY KEY (PID)
)

CREATE TABLE DETAIL (
	O_ID VARCHAR(10),
	P_ID VARCHAR(10)

	CONSTRAINT PK_DETAIL
	PRIMARY KEY (O_ID, P_ID)
)

ALTER TABLE ORDERING
ADD
	CONSTRAINT FK_O_C
	FOREIGN KEY (CUSTOMER_ID)
	REFERENCES CUSTOMER

ALTER TABLE DETAIL
ADD
	CONSTRAINT FK_D_O
	FOREIGN KEY(O_ID)
	REFERENCES ORDERING,

	CONSTRAINT FK_D_P
	FOREIGN KEY (P_ID)
	REFERENCES PRODUCT


INSERT CUSTOMER
VALUES
	(1, 'Brain', 'Chicago'),
	(2, 'Jane', 'Houston'),
	(3, 'Katie', 'Houston'),
	(4, 'John', 'Houston'),
	(5, 'Leo', 'San Jose')

INSERT PRODUCT
VALUES
	('P1', 'Cinderella', 'Books'),
	('P2', 'Dell XYZ', 'Computers'),
	('P3', 'Aladdin', 'Books'),
	('P4', 'HP 123', 'Computers')

INSERT ORDERING
VALUES
	('001', 'May 30', 2, 200),
	('002', 'June 8', 3, 500),
	('003', 'June 12', 2, 100),
	('004', 'May 30', 4, 300),
	('005', 'June 14', 2, 400),
	('006', 'May 30', 3, 300)

INSERT DETAIL
VALUES
	('001', 'P1'),
	('001', 'P3'),
	('002', 'P2'),
	('003', 'P2'),
	('004', 'P2'),
	('005', 'P2'),
	('006', 'P1')



-- Q1: Find all customers in Chicago
SELECT *
FROM CUSTOMER
WHERE CITY = 'Chicago'

-- Q3: Find name of customers who don't make any order
SELECT DISTINCT C.NAME
FROM CUSTOMER C
WHERE NOT EXISTS ( SELECT *
				   FROM ORDERING O
				   WHERE C.ID = O.CUSTOMER_ID)

-- Q5: Find name of customers whom every amounts of their orders >= 300
SELECT DISTINCT C.NAME
FROM CUSTOMER C
JOIN ORDERING O
ON C.ID = O.CUSTOMER_ID
GROUP BY C.ID, C.NAME
HAVING MIN(O.AMOUNT) >= 300


-- Q7: Find name of customers who have bought all items
SELECT DISTINCT C.NAME
FROM CUSTOMER C
JOIN ORDERING O
ON C.ID = O.CUSTOMER_ID
JOIN DETAIL D
ON O.O_ID = D.O_ID
GROUP BY C.ID, C.NAME
HAVING COUNT(DISTINCT D.P_ID) = (SELECT COUNT(PID) FROM PRODUCT)


-- Q9: Find name of customers who bought "Dell XYZ"
SELECT DISTINCT C.NAME
FROM CUSTOMER C
JOIN ORDERING O
ON C.ID = O.CUSTOMER_ID
JOIN DETAIL D
ON O.O_ID = D.O_ID
WHERE D.P_ID = 'P2'


-- Q11: For each customer, show id, name and the number of their orders
SELECT C.ID, C.NAME, COUNT(O.O_ID) AS NumberOfOrders
FROM CUSTOMER C
JOIN ORDERING O
ON C.ID = O.CUSTOMER_ID
GROUP BY C.ID, C.NAME

-- Q13: Show the date and the sales for each day
SELECT DATE, SUM(AMOUNT) AS TotalSales
FROM ORDERING
GROUP BY DATE


-- Q15:	Which city has the most sales
SELECT C.CITY
FROM CUSTOMER C
JOIN ORDERING O 
ON C.ID = O.CUSTOMER_ID
GROUP BY C.CITY
HAVING COUNT(O.AMOUNT) = ( SELECT MAX(OrdersCount)
				           FROM ( SELECT COUNT(O.AMOUNT) AS OrdersCount
								  FROM CUSTOMER C
								  JOIN ORDERING O ON C.ID = O.CUSTOMER_ID
								  GROUP BY C.CITY
								) AS CityOrdersCount
						 )


-- Q17:  For each customer, show id, name and the number of their orders
SELECT C.ID, C.NAME, COUNT(O.O_ID) AS NumberOfOrders
FROM CUSTOMER C
LEFT JOIN ORDERING O
ON C.ID = O.CUSTOMER_ID
GROUP BY C.ID, C.NAME



