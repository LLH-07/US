-- 1. Cho biết thông tin mã và tên người quản lý của giáo viên tham gia nhiều đề tài thuộc chủ đề giáo dục nhất.
SELECT QL.MAGV, QL.HOTEN
FROM GIAOVIEN QL
JOIN GIAOVIEN GV ON GV.GVQLCM = QL.MAGV
JOIN THAMGIADT TG ON TG.MAGV = GV.MAGV
JOIN DETAI DT ON TG.MADT = DT.MADT
JOIN CHUDE CD ON CD.MACD = DT.MACD
WHERE CD.TENCD LIKE N'%Giáo dục%'
GROUP BY GV.MAGV, QL.MAGV, QL.HOTEN
HAVING COUNT(DISTINCT TG.MADT) = (
	SELECT MAX(SLDT)
	FROM ( SELECT COUNT(DISTINCT TG.MADT) AS SLDT
		   FROM THAMGIADT TG1
		   JOIN DETAI DT1 ON TG1.MADT = DT1.MADT
		   JOIN CHUDE CD1 ON CD1.MACD = DT1.MACD
		   WHERE CD1.TENCD LIKE N'%Giáo dục%'
		   GROUP BY TG1.MAGV) MAX_SLDT)
GO

-- 2. Cho biết trưởng khoa của giáo viên chủ nhiệm đề tài có tất cả giáo viên có họ Nguyễn hơn 30 tuổi tham gia.
SELECT TK.*
FROM GIAOVIEN CNDT
JOIN BOMON BM ON BM.MABM = CNDT.MABM
JOIN KHOA K ON K.MAKHOA = BM.MAKHOA
JOIN DETAI DT ON CNDT.MAGV = DT.GVCNDT
JOIN GIAOVIEN TK ON TK.MAGV = K.TRUONGKHOA
WHERE NOT EXISTS ( SELECT GV.MAGV
				   FROM GIAOVIEN GV
				   WHERE GV.HOTEN LIKE N'%Nguyễn%' and DATEDIFF(YEAR, GV.NGAYSINH, GETDATE()) > 30
				   EXCEPT
				   SELECT TG.MAGV
				   FROM THAMGIADT TG
				   WHERE TG.MADT = DT.MADT
				   )
GO


-- 3.
CREATE OR ALTER
FUNCTION TinhSLCV(@MADT VARCHAR(5), @MAGV VARCHAR(5))
RETURNS int
AS
BEGIN
	RETURN (SELECT ISNULL(COUNT(*), 0)
			FROM CONGVIEC CV
			JOIN THAMGIADT TG ON CV.MADT = TG.MADT AND CV.STT = TG.STT
			JOIN GIAOVIEN GV ON GV.MAGV = TG.MAGV
			WHERE TG.MADT = @MADT AND GV.MAGV = @MAGV)
END
GO

CREATE OR ALTER
PROCEDURE sp_ThemPC_22127117
	@MAGV VARCHAR(5),
	@MADT VARCHAR(5),
	@STT INT,
	@PHUCAP FLOAT,
	@KQ INT OUT
AS
BEGIN
	IF @MAGV IS NULL
	IF NOT EXISTS (SELECT * FROM GIAOVIEN WHERE MAGV = @MAGV)
		SET @KQ = 0;
		THROW 50001, N'MÃ GIÁO VIÊN KHÔNG TỒN TẠI', 1; 
		return 0

	IF @MADT IS NULL
	IF NOT EXISTS (SELECT * FROM DETAI WHERE MADT = @MADT)
		SET @KQ = 0;
		THROW 50002, N'MÃ ĐỀ TÀI KHÔNG TỒN TẠI', 1; 
		return 0

	IF @PHUCAP <= 0
		SET @KQ = 0;
		THROW 50003, N'PHỤ CẤP KHÔNG HỢP LỆ', 1; 
		return 0


	DECLARE @SLCVTG INT = dbo.TinhSLCV(@MADT, @MAGV)
	IF @SLCVTG > 3
		SET @KQ = 0;
		THROW 50004, N'THAM GIA TRÊN 3 CÔNG VIỆC', 1; 
		return 0


	IF EXISTS (SELECT DISTINCT GV.*
			   FROM GIAOVIEN GV
			   JOIN BOMON BM ON GV.MABM = BM.MABM
			   JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
			   JOIN THAMGIADT TG ON TG.MAGV = GV.MAGV
			   WHERE EXISTS(SELECT DISTINCT TBM.*
							FROM GIAOVIEN TBM
							JOIN THAMGIADT TG1 ON TG1.MAGV = TBM.MAGV
							WHERE TBM.MAGV = BM.TRUONGBM AND TG1.MADT = TG.MADT AND TG1.PHUCAP <= TG.PHUCAP)
			   OR EXISTS(SELECT DISTINCT TK.*
							FROM GIAOVIEN TK
							JOIN THAMGIADT TG2 ON TG2.MAGV = TK.MAGV
							WHERE TK.MAGV = K.TRUONGKHOA AND TG2.MADT = TG.MADT AND TG2.PHUCAP <= TG.PHUCAP))

		SET @KQ = 0;
		THROW 50005, N'LƯƠNG GIÁO VIÊN LỚN HƠN', 1; 
		return 0

	INSERT INTO THAMGIADT(MAGV, MADT, STT, PHUCAP)
	VALUES (@MAGV, @MADT, @STT, @PHUCAP)
	return 1
END
GO































USE QLDT
/*1.(2.5đ) Cho biết thông tin mã và tên người quản lý của giáo viên tham gia nhiều đề tài 
-- thuộc chủ đề giáo dục nhất.*/ 
SELECT QL.MAGV, QL.HOTEN 
FROM GIAOVIEN QL JOIN GIAOVIEN GV ON QL.MAGV = GV.GVQLCM
                 JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
                 JOIN DETAI DT ON TG.MADT = DT.MADT 
                 JOIN CHUDE CD ON DT.MACD = CD.MACD
WHERE CD.TENCD LIKE N'%Giáo dục%'
GROUP BY GV.MAGV, QL.MAGV, QL.HOTEN 
HAVING COUNT(DISTINCT TG.MADT) >= ALL (SELECT COUNT (DISTINCT TG2.MADT)
                                       FROM THAMGIADT TG2 JOIN DETAI DT2 ON TG2.MADT = DT2.MADT 
                                                          JOIN CHUDE CD2 ON DT2.MACD = CD2.MACD
                                       WHERE CD2.TENCD LIKE N'%Giáo dục%'
                                       GROUP BY TG2.MAGV)


/*2.(2.5đ) Cho biết trưởng khoa của giáo viên chủ nhiệm đề tài có 
-- tất cả giáo viên có họ Nguyễn hơn 30 tuổi tham gia.*/
--Phép chia: ...."đề tài có tất cả giáo viên họ Nguyễn hơn 30 tuổi tham gia"
--S: GIAOVIEN(MAGV) (HỌ NGUYỄN, >30 TUỔI)
--T: DETAI(MADT)
--R: THAMGIADT(MAGV, MADT)
GO
CREATE FUNCTION UF_GETAGE(@MSGV CHAR(10))
RETURNS INT 
AS 
    BEGIN 
        DECLARE @TUOI INT 
        SELECT @TUOI = DATEDIFF(YY, NGSINH, GETDATE())
        FROM GIAOVIEN WHERE MAGV = @MSGV

        RETURN @TUOI 
    END 
GO 

--CÁCH 1: DÙNG GROUP BY ... HAVING
SELECT TK.MAGV, TK.HOTEN
FROM DETAI DT JOIN THAMGIADT TG ON DT.MADT = TG.MADT -- THỰC HIỆN PHÉP CHIA
              JOIN GIAOVIEN GV ON GV.MAGV = TG.MAGV 
              JOIN GIAOVIEN CN ON DT.GVCNDT = CN.MAGV -- LẤY KẾT QUẢ PHÉP CHIA DT KẾT TÌM RA YÊU CẦU TRUY VẤN
              JOIN BOMON BM ON CN.MABM = BM.MABM 
              JOIN KHOA K ON K.MAKHOA = BM.MAKHOA 
              JOIN GIAOVIEN TK ON TK.MAGV = K.TRUONGKHOA
WHERE GV.HOTEN LIKE N'NGUYỄN%' AND DBO.UF_GETAGE(GV.MAGV) > 30
GROUP BY DT.MADT, TK.MAGV, TK.HOTEN -- ỨNG VỚI MỖI ĐỀ TÀI (T) ĐẾM SỐ GV HỌ NGUYỄN TRÊN 30 TUỔI (S) THAM GIA
HAVING COUNT(DISTINCT TG.MAGV) = (SELECT COUNT(*) FROM GIAOVIEN -- ĐẾM S: SỐ LƯỢNG GIÁO VIÊN HỌ NGUYỄN TRÊN 30 TUỔI
                                  WHERE HOTEN LIKE N'NGUYỄN%' AND DBO.UF_GETAGE(MAGV) > 30)

--CÁCH 2: DÙNG NOT EXIST ... EXCEPT
SELECT TK.MAGV, TK.HOTEN
FROM DETAI DT JOIN GIAOVIEN CN ON DT.GVCNDT = CN.MAGV -- LẤY KẾT QUẢ PHÉP CHIA DT KẾT TÌM RA YÊU CẦU TRUY VẤN
              JOIN BOMON BM ON CN.MABM = BM.MABM 
              JOIN KHOA K ON K.MAKHOA = BM.MAKHOA 
              JOIN GIAOVIEN TK ON TK.MAGV = K.TRUONGKHOA
WHERE NOT EXISTS (SELECT GV.MAGV FROM GIAOVIEN GV -- LÁY DS MÃ GIÁO VIÊN HỌ NGUYỄN TRÊN 30 TUỔI
                  WHERE GV.HOTEN LIKE N'NGUYỄN%' AND DBO.UF_GETAGE(GV.MAGV) > 30
                  EXCEPT 
                  SELECT TG.MAGV FROM THAMGIADT TG  -- LẤY DS MÃ GIÁO VIÊN CÓ THAM GIA DT
                  WHERE TG.MADT = DT.MADT)
    AND EXISTS (SELECT GV.MAGV -- KIỂM TRA S KHÁC RỖNG
                FROM GIAOVIEN GV 
                WHERE GV.HOTEN LIKE N'NGUYỄN%' AND DBO.UF_GETAGE(GV.MAGV) > 30)


/*3.(2.5đ) Cài đặt stored procedure sp_ThemPC_MSSV (thay MSSV bằng mã số của sinh viên) nhận vào mã giáo viên, mã đề tài, số thứ tự, phụ cấp và thêm phân công này cho giáo viên thoả các điều kiện sau:
- Giáo viên và đề tài phải tồn tại.
- Mức phụ cấp của đề tài phải > 0.
- Trong một đề tài, mỗi giáo viên chỉ được tham gia tối đa 3 công việc (sử dụng function để đếm số lượng công việc cho một đề tài mà một giáo viên tham gia).
- Trong một đề tài, nếu có trưởng bộ môn hay trưởng khoa của giáo viên cùng tham gia thì mức phụ cấp của giáo viên không được vượt quá trưởng bộ môn hay trưởng khoa của họ.
Khi các điều kiện thoả mãn thì thêm phân công vào và trả về 1 báo hiệu thành công, ngược lại trả về mã lỗi thất bại. Lưu ý: sinh viên cần có chú thích ghi chú các bước làm.
*/
GO
-- Đếm số lượng công việc cho một đề tài mà một giáo viên tham gia
CREATE FUNCTION UF_COUNTJOB(@MSGV CHAR(10), @MADT CHAR(10))
RETURNS INT 
AS 
    BEGIN 
        DECLARE @SL INT 
        SELECT @SL = ISNULL(COUNT(*),0)
        FROM THAMGIADT WHERE MAGV = @MSGV AND MADT = @MADT
        RETURN @SL
    END 
GO 

CREATE PROCEDURE sp_ThemPC_3333 @MAGV CHAR(10), @MADT CHAR(5), @STT INT, @PHUCAP DECIMAL(5,3)
AS
    -- Giáo viên và đề tài phải tồn tại.
    IF (NOT EXISTS(SELECT * FROM GIAOVIEN WHERE MAGV = @MAGV) OR 
    NOT EXISTS(SELECT * FROM DETAI WHERE MADT = @MADT)) 
        RETURN 0

    -- Mức phụ cấp của đề tài phải > 0.
    IF (@PHUCAP <= 0) RETURN 0

    -- Trong một đề tài, mỗi giáo viên chỉ được tham gia tối đa 3 công việc
    IF (DBO.UF_COUNTJOB(@MAGV, @MADT) = 3) RETURN 0

    -- Trong một đề tài, nếu có trưởng bộ môn hay trưởng khoa của giáo viên cùng tham gia thì 
    -- mức phụ cấp của giáo viên không được vượt quá trưởng bộ môn hay trưởng khoa của họ.
    IF (@PHUCAP > ANY(SELECT TG.PHUCAP 
                      FROM THAMGIADT TG 
                      WHERE TG.MADT = @MADT AND 
                            TG.MAGV IN (SELECT QL.MAGV                                         
                                        FROM GIAOVIEN GV JOIN BOMON BM ON GV.MABM = BM.MABM 
                                                         JOIN KHOA K ON K.MAKHOA = BM.MAKHOA 
                                                         JOIN GIAOVIEN QL ON (QL.MAGV = K.TRUONGKHOA OR QL.MAGV = BM.TRUONGBM)
                                        WHERE GV.MAGV = @MAGV)
                    )) RETURN 0

    -- Thêm phân công khi các điều kiện thoả mãn
    INSERT INTO THAMGIADT (MAGV, MADT, STT, PHUCAP) VALUES (@MAGV, @MADT, @STT, @PHUCAP)
    RETURN 1
GO 

DECLARE @KQ INT 
EXEC @KQ = sp_ThemPC_3333 '001', '001', 3, 0.2
IF @KQ = 0 
    PRINT N'THÊM KHÔNG THÀNH CÔNG!'
ELSE 
BEGIN 
    PRINT N'THÊM THÀNH CÔNG'
    SELECT * FROM THAMGIADT
END

/*4.	(2.5đ) Cài đặt stored procedure sp_InKhoa_MSSV (thay MSSV bằng mã số của sinh viên) nhận vào mã khoa và in thông tin của khoa theo mẫu sau. 
---Khoa: mã khoa – tên khoa
---Trưởng khoa: mã gv – tên gv
---Số lượng bộ môn thuộc khoa: … 
---Số lượng giảng viên của khoa: …
*/
GO
CREATE OR ALTER PROCEDURE sp_InKhoa_3333 @MAKHOA CHAR(10)
AS 
    IF (NOT EXISTS(SELECT * FROM KHOA WHERE MAKHOA = @MAKHOA))
        PRINT N'KHOA KHÔNG TỒN TẠI!'
    ELSE 
    BEGIN 
        DECLARE @TENKHOA NVARCHAR(150), @TENTK NVARCHAR(150), @SLBM INT, @SLGV INT 

        SELECT @TENKHOA = K.TENKHOA, 
               @TENTK = ISNULL(TK.HOTEN, '-'), 
               @SLBM = COUNT(DISTINCT BM.MABM), 
               @SLGV = COUNT(GV.MAGV)
        FROM KHOA K LEFT JOIN GIAOVIEN TK ON K.TRUONGKHOA = TK.MAGV
                    LEFT JOIN BOMON BM ON K.MAKHOA = BM.MAKHOA 
                    LEFT JOIN GIAOVIEN GV ON GV.MABM = BM.MABM 
        WHERE K.MAKHOA = @MAKHOA
        GROUP BY K.MAKHOA, K.TENKHOA, TK.HOTEN

        PRINT N'KHOA:' + @TENKHOA
        PRINT N'TRƯỞNG KHOA:' + @TENTK 
        PRINT N'SỐ LƯỢNG BỘ MÔN:' + CONVERT(CHAR(3), @SLBM)
        PRINT N'SỐ LƯỢNG GIẢNG VIÊN:' + CONVERT(CHAR(3), @SLGV)

    END 
GO 

EXEC sp_InKhoa_3333 'CNTT'