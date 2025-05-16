-- MÃ LỚP: 22CLC03
-- MSSV: 22127117
-- Họ tên: Lý Liên Hoa 
-- Mã đề: 124
-- Vị trí: cột 7 - dòng 2

-- 1:
SELECT KH.*, COUNT(TK.MaTK) AS SoLuongTK, SUM(TK.SoDu) AS TongSoDu
FROM KHACHHANG KH
JOIN TAIKHOAN TK ON KH.MaKH = TK.MaKH
GROUP BY KH.MaKH, KH.HoTen, KH.NgaySinh, KH.CMND, KH.DiaChi
HAVING COUNT(DISTINCT TK.LoaiTK) = 3

-- 2:
SELECT TK.*
FROM TAIKHOAN TK
JOIN GIAODICH GD ON TK.MaTK = GD.MaTK
JOIN LOAITAIKHOAN LTK ON LTK.MaLoai = TK.LoaiTK
WHERE LTK.TenLoai LIKE N'%Thanh toán' AND GD.GhiChu LIKE N'%Rút tiền%'
	  AND YEAR(GD.ThoiGianGD) = YEAR(GETDATE())
	  AND NOT EXISTS (SELECT * 
					  FROM GIAODICH GD1 
					  JOIN TAIKHOAN TK1 ON TK1.MaTK = GD1.MaTK
					  WHERE TK1.MaTK = TK.MaTK AND GD.GhiChu NOT LIKE N'%Rút tiền%')

--3:
GO
CREATE OR ALTER
PROCEDURE sp_ThemGD_22127117
	@MAGD INT,
	@MATK CHAR(12),
	@SOTIEN INT,
	@GC NVARCHAR(50)
AS
BEGIN
	-- Mã giao dịch chưa tồn tại
	IF EXISTS(SELECT * FROM GIAODICH WHERE MaGD = @MAGD)
		return 0
	
	-- Số tiền <> 0
	IF @SOTIEN = 0
		return 0
	
	-- Số tiền > 0 thì ghi chú phải là nạp tiền/ chuyển khoản/ gửi tiền
	IF @SOTIEN > 0 AND (@GC NOT LIKE N'%Nạp tiền%' OR @GC NOT LIKE N'%Chuyển khoản%' OR @GC NOT LIKE N'%Gửi tiền%')
		return 0

	-- Số tiền < 0 thì ghi chú phải là rút tiền/ thanh toán/ chuyển khoản
	IF @SOTIEN < 0 AND (@GC NOT LIKE N'%Rút tiền%' OR @GC NOT LIKE N'%Chuyển khoản%' OR @GC NOT LIKE N'%Thanh toán%')
		return 0

	-- Mã tài khoản tồn tại
	IF EXISTS(SELECT * FROM TAIKHOAN WHERE MaTK = @MATK)
		-- Ghi chú = thanh toán ==> Tài khoản thuộc loại thanh toán
		IF (@GC LIKE N'%Thanh toán%') AND (@MATK NOT IN (SELECT TK.MaTK 
														 FROM TAIKHOAN TK
														 JOIN LOAITAIKHOAN LTK ON TK.LoaiTK = LTK.MaLoai
														 WHERE @MATK = TK.MaTK AND LTK.TenLoai LIKE N'%Thanh toán%'))
			return 0

		-- Ghi chú != thanh toán ==> Tài khoản thuộc loại không kỳ hạn
		IF (@GC NOT LIKE N'%Thanh toán%') AND (@MATK NOT IN (SELECT TK.MaTK 
															 FROM TAIKHOAN TK
															 JOIN LOAITAIKHOAN LTK ON TK.LoaiTK = LTK.MaLoai
															 WHERE @MATK = TK.MaTK AND LTK.TenLoai LIKE N'%Không kỳ hạn%'))
			return 0

	DECLARE @TGGD DATE = GETDATE()

	INSERT INTO GIAODICH(MaGD, MaTK, SoTien, ThoiGianGD, GhiChu)
	VALUES (@MAGD, @MATK, @SOTIEN, @TGGD, @GC)
	return 1
END 
GO


-- 4:
CREATE OR ALTER
FUNCTION DEMGD (@MATK CHAR(12))
RETURNS INT
BEGIN
	RETURN (SELECT ISNULL(COUNT(GD.MaGD), 0)
			FROM GIAODICH GD
			WHERE GD.MaTK = @MATK)
END
GO

CREATE OR ALTER
FUNCTION DEMGDR (@MATK CHAR(12))
RETURNS INT
BEGIN
	RETURN (SELECT ISNULL(COUNT(GD.MaGD), 0)
			FROM GIAODICH GD
			WHERE GD.MaTK = @MATK AND GD.GhiChu LIKE N'%Rút tiền%')
END
GO

CREATE OR ALTER
FUNCTION DEMGDN (@MATK CHAR(12))
RETURNS INT
BEGIN
	RETURN (SELECT ISNULL(COUNT(GD.MaGD), 0)
			FROM GIAODICH GD
			WHERE GD.MaTK = @MATK AND GD.GhiChu LIKE N'%Nạp tiền%')
END
GO

CREATE OR ALTER
PROCEDURE sp_TaiKhoanView_22127117
	@MATK CHAR(12)
AS
BEGIN
	IF NOT EXISTS (SELECT * FROM TAIKHOAN WHERE MaTK = @MATK)
		Throw 50001, N'Mã tài khoản không tồn tại', 1;

	DECLARE @HOTEN NVARCHAR(30) = (SELECT KH.HoTen FROM KHACHHANG KH JOIN TAIKHOAN TK ON TK.MaKH = KH.MaKH WHERE TK.MaTK = @MATK),
			@SODU INT = (SELECT SUM(TK.SoDu) FROM TAIKHOAN TK),
			@SLGD INT = DBO.DEMGD(@MATK),
			@SLGDR INT = DBO.DEMGDR(@MATK),
			@SLGDN INT = DBO.DEMGDN(@MATK)

	PRINT N'Mã TK: ' + @MATK
	PRINT N'Họ tên KH: ' + @HOTEN
	PRINT N'Số dư: ' + CONVERT(CHAR(15), @SODU)
	PRINT N'Số lượng giao dịch: ' + CONVERT(CHAR(15), @SLGD)
	PRINT N'Số lượng giao dịch rút: ' + CONVERT(CHAR(15), @SLGDR)
	PRINT N'Số lượng giao dịch nạp: ' + CONVERT(CHAR(15), @DLGDN)
END
GO