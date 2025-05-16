GO
-- j. Xuất ra toàn bộ danh sách giáo viên
CREATE OR ALTER FUNCTION DSGV()
RETURNS table
AS
	return (select * from GIAOVIEN)

GO

-- k. Tính số lượng đề tài mà một giáo viên đang thực hiện
CREATE OR ALTER FUNCTION TinhSLDTTG(@MAGV VARCHAR(5))
RETURNS int
AS
BEGIN
	return (SELECT ISNULL(COUNT(DISTINCT tg.MADT), 0)FROM GIAOVIEN GV
							   JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
							   WHERE GV.MAGV = @MAGV
							   GROUP BY (GV.MAGV))
END
GO

-- l. In ra thông tin chi tiết của một giáo viên (sử dụng lệnh print): Thông tin cá nhân, Số lượng đề tài tham gia, Số lượng thân nhân của giáo viên đó
CREATE OR ALTER FUNCTION TinhSLTN(@MAGV VARCHAR(5))
RETURNS int
AS
BEGIN
	return (SELECT ISNULL(COUNT(NT.TEN), 0) FROM GIAOVIEN GV
							   JOIN NGUOITHAN NT ON GV.MAGV = NT.MAGV
							   WHERE GV.MAGV = @MAGV
							   GROUP BY (GV.MAGV))
END
GO

CREATE OR ALTER
PROCEDURE XuatThongTinGV
	@MAGV VARCHAR(5)

AS
DECLARE
	@HOTEN NVARCHAR(30),
	@LUONG INT,
	@PHAI NVARCHAR(5),
	@NGSINH DATE,
	@DIACHI NVARCHAR(50),
	@GVQLCM VARCHAR(5),
	@MABM NVARCHAR(10),
	@SLDTTG INT,
	@SLTN INT


SELECT @HOTEN = HOTEN, @LUONG = LUONG, @PHAI = PHAI, @NGSINH = NGSINH,
	   @DIACHI = DIACHI, @GVQLCM = GVQLCM, @MABM = MABM,
	   @SLDTTG = dbo.fTinhSLDTTG(@MAGV), @SLTN = dbo.fTinhSLTN(@MAGV)
FROM GIAOVIEN 
WHERE @MAGV = MAGV

print N'Họ tên: ' + @HOTEN
print N'Lương: ' + cast(@LUONG as VARCHAR(10))
print N'Phái: ' + @PHAI
print N'Ngày sinh: ' + cast(@NGSINH as VARCHAR(15))
print N'Địa chỉ: ' + @DIACHI
print N'Giáo viên quản lý chuyên môn: ' + cast(@GVQLCM as VARCHAR(5))
print N'Mã bộ môn: ' + cast(@MABM as VARCHAR(10))
print N'Số lượng đề tài tham gia: ' + cast(@SLDTTG as VARCHAR(10))
print N'Số lượng thân nhân: ' + cast(@SLTN as VARCHAR(10))

GO


-- m. Kiểm tra xem một giáo viên có tồn tại hay không (dựa vào MAGV)
CREATE OR ALTER
PROCEDURE KtraGVTonTai
	@MAGV VARCHAR(5)
AS
	IF (EXISTS(SELECT * FROM GIAOVIEN WHERE MAGV = @MAGV))
		print N'Giáo viên tồn tại!'
	ELSE
		print N'Giáo viên ' + @MAGV + N' không tồn tại!'
GO

-- n. Kiểm tra quy định của một giáo viên: Chỉ được thực hiện các đề tài mà bộ môn của giáo viên đó làm chủ nhiệm
CREATE OR ALTER
PROCEDURE KtraQuyDinhGV
	@MAGV VARCHAR(5),
	@KQ NVARCHAR(20) OUT
AS
	SET @KQ = 0
	IF(EXISTS(SELECT GV.*
			  FROM GIAOVIEN GV
			  JOIN BOMON BM ON GV.MABM = BM.MABM
			  JOIN THAMGIADT TG ON TG.MAGV = GV.MAGV
			  JOIN DETAI DT ON TG.MADT = DT.MADT
			  WHERE BM.TRUONGBM = DT.GVCNDT and GV.MAGV = @MAGV))
	BEGIN
		SET @KQ = N'Hợp quy định!';
		print @KQ;
	END

	ELSE
	BEGIN
		SET @KQ = N'Không hợp quy định!';
		print @KQ;
	END
GO


-- o. Thực hiện thêm một phân công cho giáo viên thực hiện một công việc của đề tài:
	-- Kiểm tra thông tin đầu vào hợp lệ: Giáo viên phải tồn tại, công việc phải tồn tại, thời gian tham gia phải > 0
	-- Kiểm tra quy định ở câu n
CREATE OR ALTER PROCEDURE KtraQuyDinhGV_2
    @MAGV VARCHAR(5),
    @MADT VARCHAR(5),
    @TENCV NVARCHAR(50),
    @NGAYBD DATE, 
    @NGAYKT DATE
AS
BEGIN
    DECLARE @KQ NVARCHAR(20);
    
    IF (NOT EXISTS(SELECT * FROM GIAOVIEN WHERE MAGV = @MAGV))
        THROW 50001, N'Giáo viên không tồn tại!', 1;
    
    IF (NOT EXISTS(SELECT * FROM CONGVIEC WHERE MADT = @MADT AND TENCV = @TENCV))
        THROW 50002, N'Công việc không tồn tại!', 1;
    
    IF (DATEDIFF(DAY, @NGAYBD, @NGAYKT) <= 0)
        THROW 50003, N'Thời gian tham gia không hợp lệ!', 1;
    
    EXEC KtraQuyDinhGV @MAGV, @KQ OUT
    print @KQ
END
GO


-- p. Thực hiện xóa một giáo viên theo mã. Nếu giáo viên có thông tin liên quan (Có thân nhân, có làm đề tài, ...)
CREATE OR ALTER
PROCEDURE XoaGV
	@MAGV VARCHAR(5)
AS
	IF (EXISTS(SELECT * FROM NGUOITHAN WHERE MAGV = @MAGV))
		THROW 50004, N'Giáo viên có thân nhân!', 1;

	IF (EXISTS(SELECT * FROM THAMGIADT WHERE MAGV = @MAGV))
		THROW 50005, N'Giáo viên có làm đề tài!', 1;

	DELETE FROM GIAOVIEN WHERE MAGV = @MAGV
	print N'Xóa giáo viên thành công!'
GO



-- q. In ra danh sách giáo viên của một phòng ban nào đó cùng với số lượng đề tài mà giáo viên tham gia,
-- số thân nhân, số giáo viên mà giáo viên đó quản lý nếu có, ...
CREATE OR ALTER FUNCTION TinhSLGVQL(@MAGVQL VARCHAR(5))
RETURNS int
AS
BEGIN
	RETURN (SELECT ISNULL(COUNT(MAGV), 0)  FROM GIAOVIEN WHERE GVQLCM = @MAGVQL GROUP BY GVQLCM)
END
GO

CREATE OR ALTER FUNCTION DSGV_BAN(@MAKHOA VARCHAR(5))
RETURNS table
AS
	RETURN (SELECT GV.*, 
				   ISNULL(dbo.TinhSLDTTG(GV.MAGV), 0) AS SoLuongDeTaiThamGia,
				   ISNULL(dbo.TinhSLTN(GV.MAGV), 0) AS SoLuongThanNhan, 
				   ISNULL(dbo.TinhSLGVQL(GV.MAGV), 0) AS SoLuongGiaoVienQuanLy
			FROM GIAOVIEN GV
			JOIN BOMON BM ON GV.MABM = BM.MABM
			JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
			WHERE K.MAKHOA = @MAKHOA)
GO

-- r. Kiểm tra quy định của 2 giáo viên a, b: Nếu a là trưởng bộ môn c của b thì lương của a phải cao hơn
-- lương của b (a, b: mã giáo viên)
CREATE OR ALTER 
PROCEDURE KtraQuyDinh2GV
	@MAGV1 VARCHAR(5),
	@MAGV2 VARCHAR(5)
AS
BEGIN
	DECLARE @LUONG1 INT,
			@LUONG2 INT,
			@MABM1 VARCHAR(5),
			@MABM2 VARCHAR(5),
			@TRUONGBM2 VARCHAR(5)

	SELECT @LUONG1 = LUONG, @MABM1 = MABM 
	FROM GIAOVIEN 
	WHERE MAGV = @MAGV1
	
	SELECT @LUONG2 = GV.LUONG, @MABM2 = GV.MABM, @TRUONGBM2 = BM.TRUONGBM 
	FROM GIAOVIEN GV
	JOIN BOMON BM ON GV.MABM = BM.MABM
	WHERE MAGV = @MAGV2

	IF @MAGV1 = @TRUONGBM2
		IF @LUONG1 > @LUONG2
			print N'Hợp quy định!'
		ELSE
			print N'Không hợp quy định: Lương giáo viên A thấp hợp lương giáo viên B.'

	ELSE
		print N'Không hợp quy định: Giáo viên A không là trưởng bộ môn của giáo viên B.'
END
GO

-- s. Thêm một giáo viên: Kiểm tra quy định: Không trùng tên, tuổi > 18, lương > 0
CREATE OR ALTER
PROCEDURE ThemGV
	@MAGV VARCHAR (5),
	@HOTEN NVARCHAR(50),
	@LUONG INT,
	@PHAI NVARCHAR(5),
	@NGSINH DATE,
	@DIACHI NVARCHAR(50),
	@GVQLCM VARCHAR(5) = NULL, 
	@MABM NVARCHAR(5)
AS
BEGIN
	IF EXISTS (SELECT * FROM GIAOVIEN WHERE HOTEN = @HOTEN)
		THROW 50006, N'Trùng tên!', 1;
	
	IF (DATEDIFF(YEAR, @NGSINH, GETDATE()) <= 18)
		THROW 50007, N'Không trên 18 tuổi!', 1;

	IF @LUONG <= 0
		THROW 50008, N'Lương không hợp lệ!', 1;

	INSERT INTO GIAOVIEN (MAGV, HOTEN, NGSINH, PHAI, DIACHI, LUONG, GVQLCM, MABM)
    VALUES (@MAGV, @HOTEN, @NGSINH, @PHAI, @DIACHI, @LUONG, @GVQLCM, @MABM);

	print N'Thêm giáo viên thành công!'
END
GO


-- t. Mã giáo viên được xác định tự động theo quy tắc: Nếu đã có giáo viên 001, 002, 003 thì MAGV của gv mới
-- sẽ là 004. Nếu đã có giáo viên 001, 002, 005, thì MAGV của giáo viên mới là 003.



------------------------------------------------------------------------------------------------------------
---- FUNCTION
-- 1. Viết hàm truyền vào mã gv tính số đề tài tham gia
CREATE OR ALTER
FUNCTION TinhDTTG (@MAGV VARCHAR(5))
RETURNS int
AS
BEGIN
	RETURN (SELECT ISNULL(COUNT(DISTINCT MADT), 0) FROM THAMGIADT WHERE MAGV = @MAGV)
END
GO

-- 2. Viết hàm truyền vào mã dt tính số lượng công việc chưa hoàn thành
CREATE OR ALTER
FUNCTION TinhCVChuaHT (@MADT VARCHAR(5))
RETURNS int
AS
BEGIN
	RETURN (SELECT ISNULL(COUNT(*), 0)
			FROM CONGVIEC
			WHERE MADT = @MADT and NGAYKT > GETDATE())
END
GO

-- 3. Viết hàm truyền vào mã đt tính số lượng công việc đã hoàn thành
CREATE OR ALTER
FUNCTION TinhCVHT (@MADT VARCHAR(5))
RETURNS int
AS
BEGIN
	RETURN (SELECT ISNULL(COUNT(*), 0)
			FROM CONGVIEC
			WHERE MADT = @MADT and NGAYKT <= GETDATE())
END
GO

-- 4. viết hàm truyền vào madt xuất danh sách công việc trong đề tài 
-- (stt, tencv, số giáo viên tham gia, tổng phụ cấp)
CREATE OR ALTER
FUNCTION XuatDSCV (@MADT VARCHAR(5))
RETURNS table
AS
	RETURN (SELECT CV.SOTT, CV.TENCV, COUNT(TG.MAGV) AS SoGiaoVienThamGia, SUM(ISNULL(TG.PHUCAP, 0)) AS TongPhuCap
			FROM CONGVIEC CV
			JOIN THAMGIADT TG ON CV.MADT = TG.MADT
			JOIN DETAI DT ON TG.MADT = DT.MADT
			WHERE @MADT = CV.MADT
			GROUP BY CV.SOTT, CV.TENCV)
GO


--5. Viết hàm truyền vào MaGV tính số đề tài tham gia (GIỐNG CÂU 1)
CREATE OR ALTER
FUNCTION TinhDTTG (@MAGV VARCHAR(5))
RETURNS int
AS
BEGIN
	RETURN (SELECT ISNULL(COUNT(DISTINCT MADT), 0) FROM THAMGIADT WHERE MAGV = @MAGV)
END
GO

--6. Viết hàm truyền vào magv xuất danh sách đề tài tham gia
CREATE OR ALTER
FUNCTION XuatDSDTTG (@MAGV VARCHAR(5))
RETURNS table
AS
	RETURN (SELECT DISTINCT MADT
			FROM THAMGIADT
			WHERE MAGV = @MAGV)
GO

-------------------------------------------------------------------------------------------------------------
--1. VIẾT THỦ TỤC THÊM ĐỀ TÀI
--INPUT: CÁC THUỘC TÍNH CỦA ĐỀ TÀI
--OUTPUT: 1 -> NẾU THÊM KO ĐC, 0 -> THÊM ĐƯỢC
--ĐIỀU KIỆN:
---> MÃ ĐỀ TÀI KHÔNG ĐƯỢC TRÙNG
---> TÊN ĐỀ TÀI KHÔNG RỖNG
---> GIÁO VIÊN CHỦ NHIỆM PHẢI LÀ GIÁO VIÊN TRÊN 35 TUỔI
---> KINH PHÍ CỦA ĐỀ TÀI CẤP TRƯỜNG < 100; ĐHQG < 1000
---> NGÀY BẮT ĐẦU < NGÀY KẾT THÚC


CREATE OR ALTER
PROCEDURE ThemDT
	@MADT VARCHAR(5),
	@TENDT NVARCHAR(50),
	@CAPQL NVARCHAR(10),
	@KINHPHI INT,
	@NGAYBD DATE,
	@NGAYKT DATE, 
	@MACD NVARCHAR(5),
	@GVCNDT VARCHAR(5),
	@KQ INT OUT
AS
BEGIN
	DECLARE @TuoiGVCN INT = 0
	
	SET @KQ = 0

	IF @MADT IN (SELECT MADT FROM DETAI)
		THROW 50008, N'Trùng mã đề tài!', 1;
		SET @KQ = 1
		RETURN 1

	IF @TENDT IS NULL
		THROW 50009, N'Tên đề tài rỗng!', 1;
		SET @KQ = 1
		RETURN 1

	SELECT @TuoiGVCN = DATEDIFF(YEAR, NGSINH, GETDATE())
	FROM GIAOVIEN
	WHERE MAGV = @GVCNDT

	IF @TuoiGVCN <= 35
		THROW 50010, N'Tuổi giáo viên chủ nhiệm không lớn hơn 35!', 1;
		SET @KQ = 1
		RETURN 1

	IF @CAPQL = N'Trường' and @KINHPHI >= 100
		THROW 50011, N'Kinh phí đề tài cấp trường không hợp lệ!', 1;
		SET @KQ = 1
		RETURN 1

	IF @CAPQL = N'ĐHQG' and @KINHPHI >= 1000
		THROW 50012, N'Kinh phí đề tài cấp ĐHQG không hợp lệ!', 1;
		SET @KQ = 1
		RETURN 1

	IF @NGAYBD >= @NGAYKT
		THROW 50013, N'Ngày bắt đầu và ngày kết thúc không hợp lệ', 1;
		SET @KQ = 1
		RETURN 1

	
	INSERT INTO DETAI(MADT, TENDT, CAPQL, KINHPHI, NGAYBD, NGAYKT, MACD, GVCNDT)
	VALUES (@MADT, @TENDT, @CAPQL, @KINHPHI, @NGAYBD, @NGAYKT, @MACD, @GVCNDT)
	RETURN 0
END
GO
	
--2. VIẾT THỦ TỤC XÓA ĐỀ TÀI CHƯA CÓ GIÁO VIÊN THAM GIA
--INPUT: MÃ ĐT
--OUTPUT: XÓA THÀNH CÔNG HAY KHÔNG
--ĐIỀU KIỆN:
---> ĐỀ TÀI ĐÃ KẾT THÚC VÀ KHÔNG CÓ NGƯỜI THAM GIA
---> ĐỀ TÀI KHÔNG CÓ CHỦ NHIỆM

CREATE OR ALTER
PROCEDURE XoaDTChuaTG
	@MADT VARCHAR(5)

AS
BEGIN
	DECLARE @SLTG INT,
			@GVCNDT VARCHAR(5)

	IF EXISTS (SELECT * FROM CONGVIEC WHERE MADT = @MADT AND NGAYKT >= GETDATE())
		print N'Xóa không thành công!';
		THROW 50014, N'Đề tài chưa kết thúc!', 1;
		return 

	SELECT @SLTG = (SELECT COUNT(MAGV) FROM THAMGIADT WHERE MADT = @MADT),
		   @GVCNDT = GVCNDT
	FROM DETAI
	WHERE @MADT = MADT

	IF @SLTG > 0
		print N'Xóa không thành công!';
		THROW 50015, N'Có giáo viên tham gia!', 1;
		return

	IF @GVCNDT IS NOT NULL
		print N'Xóa không thành công!';
		THROW 50016, N'Có giáo viên chủ nhiệm đề tài', 1;
		return

	DELETE FROM DETAI WHERE @MADT = MADT
	print N'Xóa thành công'
END
GO


--3. VIẾT THỦ TỤC GIA HẠN ĐỀ TÀI
--INPUT: MADT, NGAYKT
--OUTPUT: GIA HẠN THÀNH CÔNG KHÔNG
--ĐIỀU KIỆN
---> ĐỀ TÀI TỒN TẠI
---> ĐỀ TÀI ĐÃ BẮT ĐẦU VÀ CHƯA KẾT THÚC
---> ĐỀ TÀI ĐÃ CÓ ÍT NHẤT 1 GIÁO VIÊN THAM GIA

CREATE OR ALTER
PROCEDURE GiaHanDT
	@MADT VARCHAR(5),
	@NGAYKT DATE
AS
BEGIN
	IF NOT EXISTS (SELECT * FROM DETAI WHERE MADT = @MADT)
		print N'Gia hạn thất bại';
		THROW 50017, N'Mã đề tài không tồn tại!', 1;
	

	IF NOT EXISTS (SELECT * FROM CONGVIEC WHERE NGAYKT > GETDATE())
		print N'Gia hạn thất bại';
		THROW 50018, N'Đề tài đã kết thúc!', 1;

	IF NOT EXISTS (SELECT * FROM THAMGIADT WHERE MADT = @MADT)
		print N'Gia hạn thất bại';
		THROW 50019, N'Không có giáo viên tham gia!', 1;
		

	UPDATE DETAI SET NGAYKT = @NGAYKT WHERE MADT = @MADT
END
GO


--4. VIẾT THỦ TỤC IN THÔNG TIN THỐNG KÊ SAU
--INPUT: MAGV
--MẪU THỐNG KÊ
---> MÃ GV:
---> HỌ TÊN:
---> TÊN BM:
---> SỐ ĐỀ TÀI THAM GIA:
---> DANH SÁCH ĐỀ TÀI THAM GIA
---	 MADT	TENDT	PHUCAP	KETQUA

CREATE OR ALTER
PROCEDURE INThongTinThongKeDTTG
	@MAGV VARCHAR(5)
AS
BEGIN
	DECLARE @HOTEN NVARCHAR(50),
			@TENBM NVARCHAR(50),
			@SoDTTG INT

	DECLARE @i INT = 1,
			@n INT

	SELECT @HOTEN = GV.HOTEN, @TENBM = BM.TENBM, @SoDTTG = COUNT(DISTINCT TG.MADT)
	FROM GIAOVIEN GV 
	JOIN BOMON BM ON GV.MABM = BM.MABM
	LEFT JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
	WHERE @MAGV = GV.MAGV 
	GROUP BY GV.HOTEN, BM.TENBM


	print N'MÃ GV: ' + @MAGV
	print N'HỌ TÊN: ' + @HOTEN
	print N'TÊN BÔ MÔN: ' + @TENBM
	print N'SỐ ĐỀ TÀI THAM GIA: ' + cast(@SoDTTG as VARCHAR(5))
	
	IF @SoDTTG > 0
	begin
		print N'Danh sách đề tài tham gia'
		print N'MADT' + space(5) + 'TENDT' + space(45) + 'PHUCAP' + space(5) + 'KETQUA'

		DECLARE @MADT VARCHAR(5),
				@TENDT NVARCHAR(50),
				@PHUCAP FLOAT,
				@KETQUA NVARCHAR(10)
	
		SET @n = @SoDTTG

		WHILE @i <= @n
			BEGIN
				SELECT @MADT = TGIA.MADT, @TENDT = TGIA.TENDT, @PHUCAP = TGIA.PHUCAP, @KETQUA = ISNULL(TGIA.KETQUA, '-')
				FROM (
					SELECT ROW_NUMBER() OVER (ORDER BY DT.MADT) AS STT, DT.MADT, DT.TENDT, TG.PHUCAP, TG.KETQUA
					FROM DETAI DT
					JOIN THAMGIADT TG ON TG.MADT = DT.MADT
					WHERE TG.MAGV = @MAGV
				) AS TGIA
				WHERE TGIA.STT = @i

				PRINT @MADT + SPACE(9 - LEN(@MADT)) + @TENDT + SPACE(50 - LEN(@TENDT)) + 
					  CAST(@PHUCAP AS NVARCHAR(10)) + SPACE(11 - LEN(CAST(@PHUCAP AS NVARCHAR(10)))) + @KETQUA

				SET @i = @i + 1
			END
		END
	end
GO


--5. VIẾT THỦ TỤC IN THÔNG TIN THỐNG KÊ SAU
--INPUT: MADT
--MẪU THỐNG KÊ
--> MADT:
--> TÊN DT:
--> TÊN GVCN:
--> SỐ CÔNG VIỆC HOÀN THÀNH:
--> SỐ CÔNG VIỆC CHƯA HOÀN THÀNH:
--> DANH SÁCH CÔNG VIỆC TRONG ĐỀ TÀI
----STT	TÊN CV	SỐGVTG	TỔNGPHUCAP	
CREATE OR ALTER
PROCEDURE INThongTinThongKeCV
	@MADT VARCHAR(5)
AS
BEGIN
	DECLARE @TENDT NVARCHAR(50),
			@TENGVCN NVARCHAR(50),
			@SoCVHT INT = dbo.TinhCVHT(@MADT),
			@SoCVChuaHT INT = dbo.TinhCVChuaHT(@MADT)

	DECLARE @i INT = 1,
			@n INT = (SELECT COUNT(*) FROM CONGVIEC WHERE MADT = @MADT)

	SELECT @TENDT = TENDT, @TENGVCN = GV.HOTEN
	FROM DETAI DT
	JOIN GIAOVIEN GV ON GV.MAGV = DT.GVCNDT
	WHERE MADT = @MADT

	print N'MÃ ĐT: ' + @MADT
	print N'TÊN ĐT: ' + @TENDT
	print N'TÊN GVCN: ' + @TENGVCN
	print N'SỐ CÔNG VIỆC HOÀN THÀNH: ' + cast(@SoCVHT as VARCHAR(5))
	print N'SỐ CÔNG VIỆC CHƯA HOÀN THÀNH: ' + cast(@SoCVChuaHT as VARCHAR(5))
	
	print N'STT' + space(5) + N'TÊN CV' + space(45) + N'SỐ GVTG' + space(5) + N'TỔNG PHỤ CẤP'

	DECLARE @STT INT,
			@TENCV NVARCHAR(50), 
			@SoGVTG INT, 
			@TongPC FLOAT
	
	WHILE @i <= @n
	begin
		SELECT @STT = CV.STT, @TENCV = CV.TENCV, @SoGVTG = CV.SLGVTG, @TongPC = CV.TPC
		FROM ( SELECT ROW_NUMBER() OVER (ORDER BY CV.SOTT) STT, CV.TENCV, COUNT(DISTINCT TG.MAGV) AS SLGVTG, SUM(TG.PHUCAP) AS TPC
			   FROM CONGVIEC CV
			   JOIN THAMGIADT TG ON CV.MADT = TG.MADT
			   WHERE @MADT = CV.MADT
			   GROUP BY CV.SOTT, CV.TENCV
			   ) AS CV
		WHERE @i = CV.STT

		PRINT CAST(@STT AS NVARCHAR(5)) + SPACE(9 - LEN(@STT)) + @TENCV + SPACE(50 - LEN(@TENCV)) + 
					  CAST(@SoGVTG AS NVARCHAR(5)) + SPACE(12 - LEN(CAST(@SoGVTG AS NVARCHAR(5)))) + CAST(@TongPC AS NVARCHAR(5))

		SET @i = @i + 1
	end


END
GO

EXEC INThongTinThongKeCV '001'