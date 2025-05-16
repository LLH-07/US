/*
MSSV : 22127117
HỌ TÊN: LÝ LIÊN HOA
*/
CREATE DATABASE QLDT
GO

USE QLDT
GO

CREATE TABLE GIAOVIEN (
	MAGV CHAR(3),
	HOTEN NVARCHAR(50),
	LUONG INT,
	PHAI NVARCHAR(3),
	NGAYSINH DATE,
	DIACHI NVARCHAR(100),
	GVQLCM CHAR(3) DEFAULT NULL,
	MABM NVARCHAR(5)

	CONSTRAINT PK_GIAOVIEN
	PRIMARY KEY (MAGV)
)

CREATE TABLE GV_DT (
	MAGV CHAR(3),
	DIENTHOAI VARCHAR(15)

	CONSTRAINT PK_GV_DT
	PRIMARY KEY(MAGV, DIENTHOAI)
)

CREATE TABLE BOMON (
	MABM NVARCHAR(5),
	TENBM NVARCHAR(50),
	PHONG VARCHAR(5),
	DIENTHOAI VARCHAR(15),
	TRUONGBM CHAR(3) DEFAULT NULL,
	MAKHOA VARCHAR(5),
	NGAYNHANCHUC DATE DEFAULT NULL

	CONSTRAINT PK_BOMON
	PRIMARY KEY (MABM)
)

CREATE TABLE KHOA (
	MAKHOA VARCHAR(5),
	TENKHOA NVARCHAR(30),
	NAMTL INT,
	PHONG VARCHAR(5),
	DIENTHOAI VARCHAR(15),
	TRUONGKHOA CHAR(3),
	NGAYNHANCHUC DATE

	CONSTRAINT PK_KHOA
	PRIMARY KEY (MAKHOA)
)

CREATE TABLE DETAI (
	MADT CHAR(3),
	TENDT NVARCHAR(60),
	CAPQL NVARCHAR(10),
	KINHPHI INT,
	NGAYBD DATE,
	NGAYKT DATE,
	MACD NVARCHAR(5),
	GVCNDT CHAR(3)

	CONSTRAINT PK_DETAI
	PRIMARY KEY (MADT)
)

CREATE TABLE CHUDE (
	MACD NVARCHAR(5),
	TENCD NVARCHAR(50)

	CONSTRAINT PK_CHUDE
	PRIMARY KEY (MACD)
)

CREATE TABLE CONGVIEC (
	MADT CHAR(3),
	STT INT,
	TENCV NVARCHAR(50),
	NGAYBD DATE,
	NGAYKT DATE

	CONSTRAINT PK_CONGVIEC
	PRIMARY KEY (MADT, STT)
)

CREATE TABLE THAMGIADT (
	MAGV CHAR(3),
	MADT CHAR(3),
	STT INT,
	PHUCAP FLOAT DEFAULT 0.0,
	KETQUA NVARCHAR (10) DEFAULT NULL

	CONSTRAINT PK_THAMGIADT
	PRIMARY KEY (MAGV, MADT, STT)
)

CREATE TABLE NGUOITHAN (
	MAGV CHAR(3),
	TEN NVARCHAR(10),
	NGSINH DATE,
	PHAI NVARCHAR(3)

	CONSTRAINT PK_NGUOITHAN
	PRIMARY KEY (MAGV, TEN)
)

ALTER TABLE GIAOVIEN
ADD
	CONSTRAINT FK_GV_GV
	FOREIGN KEY (GVQLCM)
	REFERENCES GIAOVIEN,

	CONSTRAINT FK_GV_BM
	FOREIGN KEY (MABM)
	REFERENCES BOMON

ALTER TABLE GV_DT
ADD 
	CONSTRAINT FK_GV_DT_GV
	FOREIGN KEY (MAGV)
	REFERENCES GIAOVIEN

ALTER TABLE BOMON
ADD 
	CONSTRAINT FK_BM_K
	FOREIGN KEY (MAKHOA)
	REFERENCES KHOA,

	CONSTRAINT FK_BM_GV
	FOREIGN KEY (TRUONGBM)
	REFERENCES GIAOVIEN(MAGV)

ALTER TABLE KHOA
ADD 
	CONSTRAINT FK_K_GV
	FOREIGN KEY (TRUONGKHOA)
	REFERENCES GIAOVIEN

ALTER TABLE DETAI
ADD 
	CONSTRAINT FK_DT_CD
	FOREIGN KEY (MACD)
	REFERENCES CHUDE,

	CONSTRAINT FK_DT_GV
	FOREIGN KEY (GVCNDT)
	REFERENCES GIAOVIEN

ALTER TABLE CONGVIEC
ADD 
	CONSTRAINT FK_CV_DT
	FOREIGN KEY (MADT)
	REFERENCES DETAI

ALTER TABLE THAMGIADT
ADD 
	CONSTRAINT FK_TG_CV
	FOREIGN KEY (MADT, STT)
	REFERENCES CONGVIEC

ALTER TABLE NGUOITHAN
ADD 
	CONSTRAINT FK_NT_GV
	FOREIGN KEY (MAGV)
	REFERENCES GIAOVIEN


INSERT GIAOVIEN
VALUES
	('001', N'Nguyễn Hoài An', 2000, N'Nam', '1973-02-15', N'25/3 Lạc Long Quân, Q.10, TP HCM', null, null),
	('002', N'Trần Trà Hương', 2500, N'Nữ', '1960-06-20', N'125 Trần Hưng Đạo, Q.1, TP HCM', null, null),
	('003', N'Nguyễn Ngọc Ánh', 2200, N'Nữ', '1975-05-11', N'12/21 Võ Văn Ngân Thủ Dức, TP HCM', null, null),
	('004', N'Trương Nam Sơn', 2300, N'Nam', '1959-06-20', N'215 Lý Thường Kiệt, TP Biên Hòa', null, null),
	('005', N'Lý Hoàng Hà', 2500, N'Nam', '1954-10-23', N'22/5 Nguyễn Xú, Q. Bình Thạnh, TP HCM', null, null),
	('006', N'Trần Bạch Tuyết', 1500, N'Nữ', '1980-05-20', N'127 Hùng Vương, TP Mỹ Tho', null, null),
	('007', N'Nguyễn An Trung', 2100, N'Nam', '1976-06-05', N'234 3/2, TP Biên Hòa', null, null),
	('008', N'Trần Trung Hiếu', 1800, N'Nam', '1977-08-06', N'22/11 Lý Thường Kiệt, TP Mỹ Tho', null, null),
	('009', N'Trần Hoàng Nam', 2000, N'Nam', '1975-11-22', N'234 Trần Não, An Phú, TP HCM', null, null),
	('010', N'Phạm Nam Thanh', 1500, N'Nam', '1980-12-12', N'221 Hùng Vương, Q.5, TP HCM', null, null)

INSERT GV_DT
VALUES
	('001', '0838912112'),
	('001', '0903123123'),
	('002', '0913454545'),
	('003', '0838121212'),
	('003', '0903656565'),
	('004', '0937125125'),
	('006', '0937888888'),
	('008', '0653717171'),
	('008', '0913232323')

INSERT KHOA
VALUES
	('CNTT', N'Công nghệ thông tin', 1995, 'B11', '0838123456', '002', '2005-02-20'),
	('HH', N'Hóa học', 1980, 'B41', '0838456456', '007', '2001-10-15'),
	('SH', N'Sinh học', 1980, 'B31', '0838454545', '004', '2000-10-11'),
	('VL', N'Vật lý', 1976, 'B21', '0838223223', '005', '2003-09-18')

INSERT BOMON
VALUES
	(N'CNTT', N'Công nghệ tri thức', 'B15', '0838126126', null, 'CNTT', null),
	(N'HHC', N'Hóa hữu cơ', 'B44', '0838222222', null, 'HH', null),
	(N'HL', N'Hóa lý', 'B42', '0838878787', null, 'HH', null),
	(N'HPT', N'Hóa phân tích', 'B43', '0838777777', '007', 'HH', '2007-10-15'),
	(N'HTTT', N'Hệ thống thông tin', 'B13', '0838125125', '002', 'CNTT', '2004-09-20'),
	(N'MMT', N'Mạng máy tính', 'B16', '0838676767', '001', 'CNTT', '2005-05-15'),
	(N'SH', N'Sinh hóa', 'B33', '0838898989', null, 'SH', null),
	(N'VLĐT', N'Vật lý điện tử', 'B23', '0838234234', null, 'VL', null),
	(N'VLƯD', N'Vật lý ứng dụng', 'B24', '0838454545', '005', 'VL', '2006-02-18'),
	(N'VS', N'Vi sinh', 'B32', '0838909090', '004', 'SH', '2007-01-01')


INSERT CHUDE
VALUES
	(N'NCPT', N'Nghiên cứu phát triển'),
	(N'QLGD', N'Quản lý giáo dục'),
	(N'ƯDCN', N'Ứng dụng công nghệ')

INSERT DETAI
VALUES
	('001', N'HTTT quản lý các trường ĐH', N'ĐHQG', 20, '2007-10-20', '2008-10-20', N'QLGD', '002'),
	('002', N'HTTT quản lý giáo vụ cho một Khoa', N'Trường', 20, '2000-10-12', '2001-10-12', N'QLGD', '002'),
	('003', N'Nghiên cứu chế tạo sợi Nanô Platin', N'ĐHQG', 300, '2008-05-15', '2010-05-15', N'NCPT', '005'),
	('004', N'Tạo vật liệu sinh học bằng màng ối người', N'Nhà nước', 100, '2007-01-01', '2009-12-31', N'NCPT', '004'),
	('005', N'Ứng dụng hóa học xanh', N'Trường', 200, '2003-10-10', '2004-12-10', N'ƯDCN', '007'),
	('006', N'Nghiên cứu tế bào gốc', N'Nhà nước', 4000, '2006-10-20', '2009-10-20', N'NCPT', '004'),
	('007', N'HTTT quản lý thư viện ở các trường ĐH', N'Trường', 20, '2009-05-10', '2010-05-10', N'QLGD', '001')

INSERT CONGVIEC
VALUES
	('001', 1, N'Khởi tạo và Lập kế hoạch', '2007-10-20', '2008-12-20'),
	('001', 2, N'Xác định yêu cầu', '2008-12-21', '2008-03-21'),
	('001', 3, N'Phân tích hệ thống', '2008-03-22', '2008-05-22'),
	('001', 4, N'Thiết kế hệ thống', '2008-05-23', '2008-06-23'),
	('001', 5, N'Cài đặt thử nghiệm', '2008-06-24', '2008-10-20'),
	('002', 1, N'Khởi tạo và Lập kế hoạch', '2009-05-10', '2009-07-10'),
	('002', 2, N'Xác định yêu cầu', '2009-07-11', '2009-10-11'),
	('002', 3, N'Phân tích hệ thống', '2009-10-12', '2009-12-20'),
	('002', 4, N'Thiết kế hệ thống', '2009-12-21', '2010-03-22'),
	('002', 5, N'Cài đặt thử nghiệm', '2010-03-23', '2010-05-10'),
	('006', 1, N'Lấy mẫu', '2006-10-20', '2007-02-20'),
	('006', 2, N'Nuôi cấy', '2007-02-21', '2008-08-21')

INSERT THAMGIADT
VALUES
	('001', '002', 1, 0, NULL),
	('001', '002', 2, 2, NULL),
	('002', '001', 4, 2, N'Đạt'),
	('003', '001', 1, 1, N'Đạt'),
	('003', '001', 2, 0, N'Đạt'),
	('003', '001', 4, 1, N'Đạt'),
	('003', '002', 2, 0, NULL),
	('004', '006', 1, 0, N'Đạt'),
	('004', '006', 2, 1, N'Đạt'),
	('006', '006', 2, 1.5, N'Đạt'),
	('009', '002', 3, 0.5, NULL),
	('009', '002', 4, 1.5, NULL)

INSERT NGUOITHAN
VALUES
	('001', N'Hùng', '1990-01-14', N'Nam'),
	('001', N'Thủy', '1994-12-08', N'Nữ'),
	('003', N'Hà', '1998-09-03', N'Nữ'),
	('003', N'Thu', '1998-09-03', N'Nữ'),
	('007', N'Mai', '2003-03-26', N'Nữ'),
	('007', N'Vy', '2000-02-14', N'Nữ'),
	('008', N'Nam', '1991-05-06', N'Nam'),
	('009', N'An', '1996-08-19', N'Nam'),
	('010', N'Nguyệt', '2006-01-14', N'Nữ')


UPDATE GIAOVIEN
SET GVQLCM = '002', MABM = N'HTTT'
WHERE MAGV = '003'

UPDATE GIAOVIEN
SET GVQLCM = '004', MABM = N'VS'
WHERE MAGV = '006'

UPDATE GIAOVIEN
SET GVQLCM = '007', MABM = N'HPT'
WHERE MAGV = '008' OR MAGV =  '010'

UPDATE GIAOVIEN
SET GVQLCM = '001', MABM = N'MMT'
WHERE MAGV = '009'

UPDATE GIAOVIEN
SET MABM = N'MMT'
WHERE MAGV = '001'

UPDATE GIAOVIEN
SET MABM = N'HTTT'
WHERE MAGV = '002'

UPDATE GIAOVIEN
SET MABM = N'VS'
WHERE MAGV = '004'

UPDATE GIAOVIEN
SET MABM = N'VLĐT'
WHERE MAGV = '005'

UPDATE GIAOVIEN
SET MABM = N'HPT'
WHERE MAGV = '007'

-- Q1: Cho giáo viên (mã gv, họ tên) có tham gia đề tài do trưởng bộ môn họ chủ nhiệm
SELECT DISTINCT GV.MAGV, GV.HOTEN
FROM GIAOVIEN GV
JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
JOIN DETAI DT ON TG.MADT = DT.MADT
JOIN BOMON BM ON DT.GVCNDT = BM.TRUONGBM
WHERE GV.MABM = BM.MABM

-- Q2: Cho trưởng khoa (mã gv, họ tên) của khoa có giáo viên "Nguyễn Thanh" làm việc
SELECT TK.MAGV, TK.HOTEN
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
JOIN GIAOVIEN TK ON K.TRUONGKHOA = TK.MAGV
WHERE GV.HOTEN = N'Nguyễn Thanh'

-- Q3: Cho bộ môn (mã bm, tên bm) có trưởng bộ môn nhỏ hơn 35 tuổi lúc nhận chức
SELECT BM.MABM, BM.TENBM
FROM BOMON BM
JOIN GIAOVIEN GV ON BM.TRUONGBM = GV.MAGV
WHERE DATEDIFF(YEAR, GV.NGAYSINH, BM.NGAYNHANCHUC) < 35

-- Q4: Cho giáo viên (mã gv, họ tên) đã từng tham gia công việc có tên là "Thiết kế" hoặc đã
-- từng chủ nhiệm đề tài có công việc có tên là "Xác định yêu cầu"
SELECT GV.MAGV, GV.HOTEN
FROM GIAOVIEN GV
WHERE EXISTS (
	SELECT *
	FROM THAMGIADT TG
	JOIN CONGVIEC CV ON TG.MADT = CV.MADT
	WHERE TG.MAGV = GV.MAGV AND CV.TENCV LIKE N'%Thiết kế%'
	)
OR EXISTS (
	SELECT *
	FROM DETAI DT
	JOIN CONGVIEC CV ON DT.MADT = CV.MADT
	WHERE DT.GVCNDT = GV.MAGV AND CV.TENCV LIKE N'%Xác định yêu cầu%'
	)

-- Q5: Cho trưởng khoa (mã gv, họ tên) có tham gia đề tài thuộc chủ đề "nghiên cứu"
-- nhưng chưa từng tham gia đề tài nào thuộc chủ đề "ứng dụng"
SELECT TK.MAGV, TK.HOTEN
FROM GIAOVIEN TK
JOIN KHOA K ON TK.MAGV = K.TRUONGKHOA
WHERE EXISTS (
	SELECT *
	FROM THAMGIADT TG
	JOIN DETAI DT ON TG.MADT = DT.MADT
	JOIN CHUDE CD ON DT.MACD = CD.MACD
	WHERE CD.TENCD LIKE N'%Nghiên cứu%' AND TG.MAGV = TK.MAGV
	)
AND NOT EXISTS (
	SELECT *
	FROM THAMGIADT TG
	JOIN DETAI DT ON TG.MADT = DT.MADT
	JOIN CHUDE CD ON DT.MACD = CD.MACD
	WHERE CD.TENCD LIKE N'%Ứng dụng%' AND TG.MAGV = TK.MAGV
	)

-- Q6: Cho giáo viên (mã gv, họ tên) của giáo viên có tham gia đề tài cấp trường
-- nhưng không chủ nhiệm đề tài nào cấp trường
SELECT DISTINCT GV.MAGV, GV.HOTEN
FROM GIAOVIEN GV
JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
JOIN DETAI DT ON TG.MADT = DT.MADT
WHERE CAPQL = N'Trường'
AND NOT EXISTS ( SELECT *
				 FROM DETAI DT
				 WHERE GV.MAGV = DT.GVCNDT AND DT.CAPQL = N'Trường')

-- Q7: Cho trưởng bộ môn (mã gv, họ tên) có chủ nhiệm ít nhất một đề tài cấp nhà nước
-- và tham gia bất kỳ đề tài nào có công việc liên quan đến "nuôi cấy"
SELECT TBM.MAGV, TBM.HOTEN
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN GIAOVIEN TBM ON BM.TRUONGBM = TBM.MAGV
WHERE EXISTS ( SELECT 1
			   FROM DETAI DT
			   WHERE DT.GVCNDT = TBM.MAGV AND DT.CAPQL = N'Nhà nước' )
AND EXISTS ( SELECT 1
			 FROM THAMGIADT TG
			 JOIN CONGVIEC CV ON TG.MADT = CV.MADT
			 WHERE TG.MAGV = TBM.MAGV AND CV.TENCV LIKE N'%Nuôi cấy%' )

-- Q8: Cho giáo viên (mã gv, họ tên) chỉ tham gia đề tài cấp nhà nước
SELECT DISTINCT GV.MAGV, GV.HOTEN
FROM GIAOVIEN GV
JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
JOIN DETAI DT ON TG.MADT = DT.MADT
WHERE DT.CAPQL = N'Nhà nước' 
AND NOT EXISTS ( SELECT 1
				 FROM THAMGIADT TG
				 JOIN DETAI DT ON TG.MADT = DT.MADT
				 WHERE TG.MAGV = GV.MAGV AND DT.CAPQL <> N'Nhà nước' )


-- Q9: Cho đề tài (mã đt, tên đt) chỉ có giáo viên có vai trò quản lý chuyên môn tham gia
SELECT DISTINCT DT.MADT, DT.TENDT
FROM GIAOVIEN GV
JOIN GIAOVIEN QLCM ON GV.GVQLCM = QLCM.MAGV
JOIN THAMGIADT TG ON QLCM.MAGV = TG.MAGV
JOIN DETAI DT ON TG.MADT = DT.MADT
WHERE GV.GVQLCM IS NOT NULL


-- Q10: Cho mã, họ tên giáo viên và số lượng giáo viên mà họ quản lý chuyên môn (nếu có)
SELECT QLCM.MAGV, QLCM.HOTEN, COUNT(GV.MAGV) AS SoluongGiaovienQuanly
FROM GIAOVIEN QLCM
LEFT JOIN GIAOVIEN GV ON QLCM.MAGV = GV.GVQLCM
GROUP BY QLCM.MAGV, QLCM.HOTEN

-- Q11: Cho mã, họ tên giáo viên, tên khoa mà giáo viên thuộc về của các giao viên từng chủ nhiệm
-- trên 2 đề tài có kinh phí >= 100 triệu
SELECT DISTINCT GV.MAGV, GV.HOTEN, K.TENKHOA
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
JOIN DETAI DT ON GV.MAGV = DT.GVCNDT
WHERE DT.KINHPHI >= 100
GROUP BY GV.MAGV, GV.HOTEN, K.TENKHOA
HAVING COUNT (DT.MADT) > 2

-- Q12: Cho mã, tên bộ môn, tên trưởng bộ môn của bộ môn có mức lương trung bình
-- của các giáo viên thấp nhất ở từng khoa
SELECT BM.MABM, BM.TENBM, TBM.HOTEN AS TenTruongBM
FROM BOMON BM
JOIN GIAOVIEN TBM ON BM.TRUONGBM = TBM.MAGV
JOIN GIAOVIEN GV ON BM.MABM = GV.MABM
GROUP BY BM.MABM, BM.TENBM, TBM.HOTEN, BM.MAKHOA
HAVING AVG(GV.LUONG) = (
	SELECT MIN(LuongTB)
	FROM (
		SELECT AVG(GV.LUONG) AS LuongTB, BM.MAKHOA
		FROM GIAOVIEN GV
		JOIN BOMON BM ON GV.MABM = BM.MABM
		GROUP BY BM.MABM, BM.MAKHOA
		) AS LuongTrungBinhBoMon
	WHERE BM.MAKHOA = LuongTrungBinhBoMon.MAKHOA
)


	-- Q13: Cho biết mã, tên khoa, tên trưởng khoa của khoa có số lượng giáo viên có tham gia các đề tài là nhiều nhất
	SELECT K.MAKHOA, K.TENKHOA, TK.HOTEN AS TenTruongKhoa
	FROM KHOA K
	JOIN BOMON BM ON K.MAKHOA = BM.MAKHOA
	JOIN GIAOVIEN TK ON K.TRUONGKHOA = TK.MAGV
	JOIN GIAOVIEN GV ON BM.MABM = GV.MABM
	JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
	GROUP BY K.MAKHOA, K.TENKHOA, TK.HOTEN
	HAVING COUNT(DISTINCT GV.MAGV) = (
		SELECT MAX(SoLuongGiaoVien)
		FROM (
			SELECT COUNT(DISTINCT GV1.MAGV) AS SoLuongGiaoVien
			FROM KHOA K1
			JOIN BOMON BM1 ON K1.MAKHOA = BM1.MAKHOA
			JOIN GIAOVIEN GV1 ON BM1.MABM = GV1.MABM
			JOIN THAMGIADT TG1 ON GV1.MAGV = TG1.MAGV
			GROUP BY K1.MAKHOA
			) AS MaxSoLuong
		)



	-- Q14: Cho mã, tên chủ đề, cấp quản lý và số lượng đề tài có kinh phí từ 100 triệu trở lên
	-- theo từng cấp quản lý của mỗi chủ đề.
	SELECT CD.MACD, CD.TENCD, DT.CAPQL, COUNT(DT.MADT) AS SoLuongDeTai
	FROM DETAI DT
	JOIN CHUDE CD ON DT.MACD = CD.MACD
	WHERE DT.KINHPHI >= 100
	GROUP BY CD.MACD, CD.TENCD, DT.CAPQL

	-- Q15: Cho mã và tên đề tài của đề tài có đông giáo viên tham gia nhất
	SELECT DT.MADT, DT.TENDT, COUNT(DISTINCT TG.MAGV) AS SoLuongGiaoVienThamGia
	FROM DETAI DT
	JOIN THAMGIADT TG ON DT.MADT = TG.MADT
	GROUP BY DT.MADT, DT.TENDT
	HAVING COUNT (DISTINCT TG.MAGV) = (
		SELECT MAX(SoLuongThamGia)
		FROM ( 
			SELECT COUNT(DISTINCT TG.MAGV) AS SoLuongThamGia
			FROM THAMGIADT TG
			GROUP BY TG.MADT
			) AS SoLuongThamGiaDeTai
		)

	-- Q16: Cho trưởng khoa (mã gv, họ tên, tên khoa) của khoa có số lượng bộ môn nhiều nhất
	-- hoặc có lương trung bình của giáo viên trong khoa là thấp nhất
	SELECT TK.MAGV, TK.HOTEN, K.TENKHOA
	FROM KHOA K
	JOIN GIAOVIEN TK ON K.TRUONGKHOA = TK.MAGV
	JOIN BOMON BM ON K.MAKHOA = BM.MAKHOA
	JOIN GIAOVIEN GV ON BM.MABM = GV.MABM
	GROUP BY TK.MAGV, TK.HOTEN, K.TENKHOA
	HAVING COUNT(DISTINCT BM.MABM) = (
		SELECT MAX(SoLuongBoMon)
		FROM (
			SELECT COUNT(BM1.MABM) AS SoLuongBoMon
			FROM BOMON BM1
			GROUP BY BM1.MAKHOA
		) AS SoLuongBoMonCuaKhoa
	)
	OR AVG(GV.LUONG) = (
		SELECT MIN(LuongTrungBinh)
		FROM (
			SELECT AVG(GV1.LUONG) AS LuongTrungBinh
			FROM GIAOVIEN GV1
			JOIN BOMON BM2 ON GV1.MABM = BM2.MABM
			GROUP BY BM2.MAKHOA
		) AS LuongTrungBinhKhoa
	)

-- Q17: Cho mã và tên giáo viên chủ nhiệm nhiều đề tài cấp nhà nước nhất
-- hoặc tham gia nhiều đề tài thuộc chủ đề giáo dục nhiều nhất
SELECT GV.MAGV, GV.HOTEN
FROM GIAOVIEN GV
LEFT JOIN DETAI DT ON GV.MAGV = DT.GVCNDT
LEFT JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
GROUP BY GV.MAGV, GV.HOTEN
HAVING
	(SELECT COUNT(*)
	FROM DETAI DT1 
	WHERE DT1.CAPQL = N'Nhà nước' AND DT1.GVCNDT = GV.MAGV) = (
		SELECT MAX(SoLuongDeTaiCapNhaNuoc)
		FROM (
			SELECT COUNT(*) AS SoLuongDeTaiCapNhaNuoc
			FROM DETAI DT2
			WHERE DT2.CAPQL = N'Nhà nước'
			GROUP BY DT2.GVCNDT
		) AS SoLuongDeTaiMax
	)
OR 
	(SELECT COUNT(*)
	FROM THAMGIADT TG1
	JOIN DETAI DT3 ON TG1.MADT = DT3.MADT 
	WHERE TG1.MAGV = GV.MAGV AND DT3.MACD = N'QLGD') = (
		SELECT MAX(SoLuongThamGiaDeTaiGiaoDuc)
		FROM (
			SELECT COUNT(*) AS SoLuongThamGiaDeTaiGiaoDuc
			FROM THAMGIADT TG2
			JOIN DETAI DT4 ON TG2.MADT = DT4.MADT
			WHERE DT4.MACD = N'QLGD'
			GROUP BY TG2.MAGV
		) AS SoLuongThamGiaMax
	)


-- Q18: Xuất mã và họ tên giáo viên thuộc khoa "Công nghệ thông tin" có tham gia tất cả các đề tài thuộc cấp ĐHQG
SELECT GV.MAGV, GV.HOTEN
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
WHERE K.TENKHOA = N'Công nghệ thông tin'
AND NOT EXISTS (
	SELECT *
	FROM DETAI DT
	WHERE DT.CAPQL = N'ĐHQG'
	AND NOT EXISTS (
		SELECT * 
		FROM THAMGIADT TG
		WHERE TG.MADT = DT.MADT AND TG.MAGV = GV.MAGV
		)
	)

-- Q19: Xuất mã, họ tên trưởng khoa có các đề tài tham gia bao phủ tất cả các chủ đề.
SELECT TK.MAGV, TK.HOTEN
FROM KHOA K
JOIN GIAOVIEN TK ON K.TRUONGKHOA = TK.MAGV
WHERE NOT EXISTS (
	SELECT *
	FROM CHUDE CD
	WHERE NOT EXISTS (
		SELECT *
		FROM DETAI DT
		JOIN THAMGIADT TG ON TG.MADT = TG.MADT
		WHERE DT.MACD = CD.MACD AND TG.MAGV = TK.MAGV
	)
)


-- Q20: Xuất mã, tên đề tài, tên công việc có tất cả giáo viên lương 2000-3000 tham gia
SELECT DT.MADT, DT.TENDT, STRING_AGG(CV.TENCV, ', ') AS TENCV
FROM GIAOVIEN GV
JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
JOIN DETAI DT ON TG.MADT = DT.MADT
JOIN CONGVIEC CV ON TG.STT = CV.STT AND TG.MADT = CV.MADT
WHERE NOT EXISTS (
	SELECT *
	FROM GIAOVIEN GV
	WHERE (GV.LUONG < 2000 OR GV.LUONG > 3000)
	AND NOT EXISTS (
		SELECT *
		FROM THAMGIADT TG
		WHERE TG.MAGV = GV.MAGV AND TG.MADT = DT.MADT
	)
)
GROUP BY DT.MADT, DT.TENDT





