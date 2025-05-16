USE QLDT
GO

--1: Cho bi?t thông tin mã và tên ng??i qu?n lý c?a giáo viên tham gia nhi?u ?? tài thu?c ch? ?? giáo d?c nh?t.
SELECT QL.MAGV, QL.HOTEN
FROM GIAOVIEN QL
JOIN GIAOVIEN GV ON GV.GVQLCM = QL.MAGV
JOIN THAMGIADT TG ON TG.MAGV = GV.MAGV
JOIN DETAI DT ON DT.MADT = TG.MADT
JOIN CHUDE CD ON CD.MACD = DT.MACD
WHERE CD.TENCD LIKE N'%Giáo d?c%'
GROUP BY QL.MAGV, QL.HOTEN, GV.MAGV
HAVING COUNT(DISTINCT TG.MADT) = (
	SELECT MAX(SLDT)
	FROM (SELECT COUNT(DISTINCT TG1.MADT) AS SLDT
		  FROM THAMGIADT TG1
		  JOIN DETAI DT1 ON TG1.MADT = DT1. MADT
		  JOIN CHUDE CD1 ON DT1.MACD = CD1.MACD
		  WHERE CD1.TENCD LIKE N'%Giáo d?c%'
		  GROUP BY TG1.MAGV) MAX_SLDT )


--2: Cho bi?t tr??ng khoa c?a giáo viên ch? nhi?m ?? tài có t?t c? giáo viên có h? Nguy?n h?n 30 tu?i tham gia.
SELECT DISTINCT TK.*
FROM GIAOVIEN GV 
JOIN THAMGIADT TG ON TG.MAGV = GV.MAGV
JOIN DETAI DT ON DT.MADT = TG.MADT
JOIN GIAOVIEN CN ON DT.GVCNDT = CN.MAGV
JOIN BOMON BM ON BM.MABM = CN.MABM
JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
JOIN GIAOVIEN TK ON TK.MAGV = K.TRUONGKHOA
WHERE GV.HOTEN LIKE N'%Nguy?n%' AND DATEDIFF(YEAR, GV.NGAYSINH, GETDATE()) > 30
AND NOT EXISTS (
	SELECT GV1.MAGV
	FROM GIAOVIEN GV1
	WHERE GV1.HOTEN LIKE N'%Nguy?n%' AND DATEDIFF(YEAR, GV1.NGAYSINH, GETDATE()) > 30
	EXCEPT
	SELECT TG1.MAGV
	FROM THAMGIADT TG1
	WHERE TG1.MADT = DT.MADT)


-- 3:
GO
CREATE OR ALTER
FUNCTION DemSLCV(@MAGV VARCHAR(5), @MADT VARCHAR(5))
RETURNS int
BEGIN
	RETURN(
		SELECT ISNULL(COUNT(DISTINCT(TG.STT)), 0)
		FROM THAMGIADT TG
		WHERE TG.MADT = @MADT AND TG.MAGV = @MAGV)
END
GO

CREATE OR ALTER
PROCEDURE sp_ThemPC_22127117
	@MAGV VARCHAR(5),
	@MADT VARCHAR(5),
	@STT INT,
	@PHUCAP FLOAT
AS
BEGIN
	-- Giáo viên và ?? tài ph?i t?n t?i
	IF (NOT EXISTS(SELECT * FROM GIAOVIEN WHERE MAGV = @MAGV)) OR
	   (NOT EXISTS(SELECT * FROM DETAI WHERE MADT = @MADT))
	   return 0

	-- M?c ph? c?p c?a ?? tài ph?i > 0
	IF @PHUCAP <= 0
		return 0

	-- Trong m?t ?? tài, m?i giáo viên ch? ???c tham gia t?i ?a 3 công vi?c
	IF DBO.DemSLCV(@MAGV, @MADT) > 3
		return 0

	-- Trong m?t ?? tài, n?u có tr??ng b? môn hay tr??ng khoa c?a giáo viên cùng tham gia thì m?c ph? c?p c?a giáo viên không ???c v??t quá tr??ng b? môn hay tr??ng khoa c?a h?.
	IF @PHUCAP > ANY(SELECT TG.PHUCAP
					 FROM THAMGIADT TG
					 WHERE TG.MAGV IN ( SELECT QL.MAGV
										FROM GIAOVIEN GV
										JOIN BOMON BM ON GV.MABM = BM.MABM
										JOIN KHOA K ON BM.MAKHOA = BM.MAKHOA
										JOIN GIAOVIEN QL ON (QL.MAGV = K.TRUONGKHOA OR QL.MAGV = BM.TRUONGBM)
										WHERE GV.MAGV = @MAGV)
					AND TG.MADT = @MADT)
		return 0

	INSERT INTO THAMGIADT(MAGV, MADT, STT, PHUCAP)
	VALUES (@MAGV, @MADT, @STT, @PHUCAP)
	return 1
END
GO


-- 4: Ca?i ???t stored procedure sp_InKhoa_MSSV (thay MSSV b?ng mã s? c?a sinh viên) nh?n vào mã khoa và in thông tin c?a khoa theo m?u sau. 
CREATE OR ALTER
PROCEDURE sp_InKhoa_22127117
	@MAKHOA VARCHAR(5)
AS
BEGIN
	IF @MAKHOA NOT IN (SELECT MAKHOA FROM KHOA)
		THROW 50001, N'Mã khoa không t?n t?i!', 1;

	DECLARE @TENKHOA NVARCHAR(40),
			@MAGV VARCHAR(5), @TENGV NVARCHAR(40),
			@SLBM INT, @SLGV INT



	SELECT @TENKHOA = K.TENKHOA, @MAGV = TK.MAGV, @TENGV = ISNULL(TK.HOTEN, '-'),
		   @SLBM = COUNT(DISTINCT BM.MABM), @SLGV = COUNT(GV.MAGV)
	FROM KHOA K
	LEFT JOIN BOMON BM ON BM.MAKHOA = K.MAKHOA
	LEFT JOIN GIAOVIEN TK ON TK.MAGV = K.TRUONGKHOA
	LEFT JOIN GIAOVIEN GV ON GV.MABM = BM.MABM
	WHERE K.MAKHOA = @MAKHOA
	GROUP BY K.MAKHOA, K.TENKHOA, TK.MAGV, TK.HOTEN

	print N'Khoa: ' + @MAKHOA + ' - ' + @TENKHOA
	print N'Tr??ng khoa: ' + @MAGV + ' - ' + @TENGV
	print N'S? l??ng b? môn thu?c khoa: ' + CONVERT(VARCHAR(3), @SLBM)
	print N'S? l??ng gi?ng viên c?a khoa: ' + CONVERT(VARCHAR(3), @SLGV)

END
GO

EXEC sp_InKhoa_22127117 'CNTT'
