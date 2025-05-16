CREATE DATABASE QLBH
GO
USE QLBH
/*
Các loại ràng buộc thuộc về bảng dữ liệu:
- Khóa PRIMARY(A1, A2, ...), UNIQUE(A1, A2, ...)
- Miền giá trị của 1 cột: NOT NULL DEFAULT VAL, CHECK(EXPRESSION)
- Tham chiếu: FOREIGN KEY(X, Y) REFERENCES TAB(A1, A2) | TAB


- NOT NULL & DEFAULT nên khai báo in line
								 .         .
								/ \       / \
							   / ^ \ ___ / ^ \          _   _    __     _   .         .
							 /"               "\       | \ / |  |__   /   \  \   _   /  ~~~~~~~~~ 
						   ""    0    .    0    ""     |     |  |__   \ _ /   \ / \ /   
	 					  """         ^         """    
							\___________________/    _ 
							 |_________________|    (  \\
							/                   \    )  |
						   /                     \  /  //
	 					  |                       |/  //
        				   \ (,,,)_________(,,,) /___//
						    
- In-table: Primary key, Unique, Check, Foreign key, Default

CẬP NHẬT DỮ LIỆU
- UPDATE 'TABLE'
- SET COL1 = ..., COL2 = ...
- WHERE COL3 = ...
*/




CREATE TABLE KH(
	MaKH CHAR(5) CONSTRAINT PK_KH PRIMARY KEY,
	HoTen NVARCHAR(150) NOT NULL,
	SoCCCD CHAR(12) NOT NULL CONSTRAINT UQ_KH_SoCCCC UNIQUE,
	Phai NCHAR(3) CONSTRAINT CK_KH_PHAI CHECK(PHAI = N'Nam' OR PHAI = N'Nữ'), --PHAI IN(N'Nam', N'Nữ')
	NgayTao DATE CONSTRAINT DF_KH_NgayTao DEFAULT GETDATE()
)

CREATE TABLE SP(
	MaSP CHAR(5),
	TenSP NVARCHAR(50) NOT NULL,
	DonGia DECIMAL(6, 2), -- Total number of digits: 6, Number of precision (afrer '.'): 2
	SLTon INT CONSTRAINT DF_SP_SLTon DEFAULT 0, 

	CONSTRAINT PK_SP PRIMARY KEY (MaSP),
	CONSTRAINT UQ_SP_TenSP UNIQUE (TenSP),
	CONSTRAINT CK_SP_DonGia CHECK (DonGia between 100 and 1000), -- DonGia >= 100 and DonGia <= 1000 
	CONSTRAINT CK_SP_SLTon CHECK (SLTon between 0 and 200)
)

CREATE TABLE DH
(
	MaDH CHAR(5) CONSTRAINT PK_DH PRIMARY KEY,
	NgayLap DATE CONSTRAINT DF_DH_NgayLap DEFAULT GETDATE(),
	MaKH CHAR(5),
	NgayGiaoDuKien DATE
)

CREATE TABLE CT_DH
(
	MaDon CHAR(5),
	MaSP CHAR(5),
	SoLuong int CONSTRAINT DF_CT_DH_SoLuong DEFAULT 1

	CONSTRAINT PK_CT_DH
	PRIMARY KEY(MaDon, MaSP)
)

ALTER TABLE DH
ADD
	CONSTRAINT FK_DH_KH
	FOREIGN KEY (MaKH)
	REFERENCES KH,

	CONSTRAINT CK_DH_NgayGiaoDuKien
	CHECK(NgayGiaoDuKien > NgayLap)

ALTER TABLE CT_DH
ADD
	CONSTRAINT FK_CT_DH_DH
	FOREIGN KEY (MaDon)
	REFERENCES DH,

	CONSTRAINT FK_CT_DH_SP
	FOREIGN KEY (MaSP)
	REFERENCES SP,

	CONSTRAINT CK_CT_DH
	CHECK(SoLuong > 0)

INSERT INTO KH VALUES ('KH001', N'Nguyễn Văn A', '007913981111', NULL, '01/22/2023'),
					  ('KH002', N'Nguyễn Văn B', '007913981112', NULL, '01/20/2023')

INSERT INTO SP

SELECT * FROM KH