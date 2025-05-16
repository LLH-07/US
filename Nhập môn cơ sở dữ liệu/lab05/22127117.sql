USE QLDT
GO

-- Q35: Cho biết mức lương cao nhất của các giảng viên
SELECT DISTINCT LUONG
FROM GIAOVIEN
WHERE LUONG = (
	SELECT MAX(LUONG)
	FROM GIAOVIEN GV1)

-- Q36: Cho biết những giáo viên có lương lớn nhất
SELECT GV.*
FROM GIAOVIEN GV
WHERE LUONG = (
	SELECT MAX(LUONG)
	FROM GIAOVIEN GV1)

-- Q37: Cho biết lương cao nhất của bộ môn 'HTTT'
SELECT DISTINCT LUONG
FROM GIAOVIEN
WHERE LUONG = (
	SELECT MAX(LUONG)
	FROM GIAOVIEN GV
	WHERE GV.MABM = 'HTTT'
	)




-- Q38: Cho biết tên giáo viên lớn tuổi nhất của bộ môn "Hệ thống thông tin"
SELECT GV.HOTEN
FROM GIAOVIEN GV
WHERE GV.MABM = 'HTTT'
AND GV.NGSINH = (
	SELECT MIN(GV1.NGSINH)
	FROM GIAOVIEN GV1
	WHERE GV1.MABM = 'HTTT'
	)

-- Q39: Cho biết giáo viên nhỏ tuổi nhất khoa "Công nghệ thông tin"
SELECT GV.*
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
WHERE K.TENKHOA = N'Công nghệ thông tin'
AND GV.NGSINH = (
	SELECT MAX(GV1.NGSINH)
	FROM GIAOVIEN GV1
	JOIN BOMON BM1 ON GV1.MABM = BM1.MABM
	JOIN KHOA K1 ON BM1.MAKHOA = K1.MAKHOA
	WHERE K1.TENKHOA = N'Công nghệ thông tin'
	)

-- Q40: Cho biết tên giáo viên và tên khoa của giáo viên có lương cao nhất
SELECT GV.HOTEN, K.MAKHOA
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
WHERE LUONG = (
	SELECT MAX(LUONG)
	FROM GIAOVIEN GV1)

-- Q41: Cho biết những giáo viên có lương cao nhất trong từng bộ môn của họ
SELECT GV.MAGV, GV.HOTEN, GV.MABM, GV.LUONG
FROM GIAOVIEN GV
WHERE GV.LUONG = (
    SELECT MAX(GV1.LUONG)
    FROM GIAOVIEN GV1
    WHERE GV1.MABM = GV.MABM)

-- Q42: Cho biết tên những đề tài mà giáo viên Nguyễn Hoài An chưa tham gia
SELECT DT.TENDT
FROM DETAI DT
WHERE DT.MADT NOT IN (
	SELECT TG1.MADT
	FROM THAMGIADT TG1
	JOIN GIAOVIEN GV1 ON TG1.MAGV = GV1.MAGV
	WHERE GV1.HOTEN = N'Nguyễn Hoài An')

-- Q43: Cho biết những đề tài mà giáo viên Nguyễn Hoài An chưa tham gia. Xuất ra tên đề tài và tên chủ nhiệm đề tài
SELECT DT.TENDT, GV.HOTEN
FROM DETAI DT
JOIN GIAOVIEN GV ON DT.GVCNDT = GV.MAGV
WHERE DT.MADT NOT IN (
	SELECT TG1.MADT
	FROM THAMGIADT TG1
	JOIN GIAOVIEN GV1 ON TG1.MAGV = GV1.MAGV
	WHERE GV1.HOTEN = N'Nguyễn Hoài An')

-- Q44: Cho biết những giáo viên khoa Công nghệ thông tin nhưng chưa tham gia đề tài nào
SELECT GV.*
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN KHOA K ON BM.MAKHOA = K.MAKHOA
WHERE K.TENKHOA = N'Công nghệ thông tin'
AND GV.MAGV NOT IN (
		SELECT TG.MAGV
		FROM THAMGIADT TG)


-- Q45: Tìm những giáo viên không tham gia bất kì đề tài nào
SELECT GV.*
FROM GIAOVIEN GV
WHERE GV.MAGV NOT IN (
	SELECT GV1.MAGV
	FROM GIAOVIEN GV1
	JOIN THAMGIADT TG ON GV1.MAGV = TG.MAGV)

-- Q46: Cho biết giáo viên có lương lớn hơn của giáo viên "Nguyễn Hoài An"
SELECT GV.*
FROM GIAOVIEN GV
WHERE GV.LUONG > (SELECT LUONG
				  FROM GIAOVIEN GV1
				  WHERE GV1.HOTEN = N'Nguyễn Hoài An')

-- Q47: Tìm những trưởng bộ môn tham gia tối thiểu 1 đề tài
SELECT GV.*
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM AND GV.MAGV = BM.TRUONGBM
WHERE EXISTS (
	SELECT *
	FROM THAMGIADT TG
	WHERE TG.MAGV = GV.MAGV)

-- Q48: Tìm những giáo viên trùng tên và trùng giới tính với giáo viên khác trong cùng bộ môn
SELECT GV.*
FROM GIAOVIEN GV
WHERE GV.HOTEN IN (
	SELECT GV1.HOTEN
	FROM GIAOVIEN GV1
	WHERE GV.PHAI = GV1.PHAI AND GV.MABM = GV1.MABM AND GV.MAGV <> GV1.MAGV)

-- Q49: Tìm những giáo viên có lương lớn hơn lương của ít nhất một giáo viên bộ môn "Công nghệ phần mềm"
SELECT GV.*
FROM GIAOVIEN GV
WHERE EXISTS (
	SELECT *
	FROM GIAOVIEN GV1
	JOIN BOMON BM ON GV1.MABM = BM.MABM
	WHERE BM.TENBM = N'Công nghệ phần mềm' AND GV.LUONG > GV1.LUONG)

-- Q50: Tìm những giáo viên có lương lớn hơn lương của tất cả giáo viên thuộc bô môn "Hệ thống thông tin"
SELECT GV.*
FROM GIAOVIEN GV
WHERE GV.LUONG > all (SELECT GV1.LUONG
					  FROM GIAOVIEN GV1
					  WHERE GV1.MABM = 'HTTT')

-- Q51: Cho biết tên khoa có đông giáo viên nhất
SELECT K.TENKHOA
FROM KHOA K
JOIN BOMON BM ON K.MAKHOA = BM.MAKHOA
JOIN GIAOVIEN GV ON BM.MABM = GV.MABM
GROUP BY K.MAKHOA, K.TENKHOA
HAVING COUNT(GV.MAGV) = (
	SELECT MAX(SLGV)
	FROM (SELECT COUNT(GV1.MAGV) SLGV
		  FROM KHOA K1
		  JOIN BOMON BM1 ON K1.MAKHOA = BM1.MAKHOA
		  JOIN GIAOVIEN GV1 ON BM1.MABM = GV1.MABM
		  GROUP BY K1.MAKHOA) SLGVLN)

-- Q52: Cho biết họ tên giáo viên chủ nhiệm nhiều đề tài nhất
SELECT GV.HOTEN
FROM GIAOVIEN GV
JOIN DETAI DT ON GV.MAGV = DT.GVCNDT
GROUP BY GV.MAGV, GV.HOTEN
HAVING COUNT(DT.MADT) = (
	SELECT MAX(SLDTCN)
	FROM (SELECT COUNT(MADT) SLDTCN
		  FROM DETAI
		  GROUP BY GVCNDT) SLDTLN)

-- Q53: Cho biết mã bộ môn có nhiều giáo viên nhất
SELECT GV.MABM
FROM GIAOVIEN GV
GROUP BY GV.MABM
HAVING COUNT(GV.MAGV) = (
	SELECT MAX(SLGV)
	FROM (SELECT COUNT(MAGV) SLGV
		  FROM GIAOVIEN
		  GROUP BY MABM) SLGVLN)

-- Q54: Cho biết tên giáo viên và tên bộ môn của giáo viên tham gia nhiều đề tài nhất
SELECT GV.HOTEN, BM.TENBM
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
GROUP BY GV.MAGV, GV.HOTEN, BM.TENBM
HAVING COUNT(TG.MADT) = (
	SELECT MAX(SLDTTG)
	FROM (SELECT COUNT(MADT) SLDTTG
		  FROM THAMGIADT 
		  GROUP BY MAGV) SLLN)

-- Q55: Cho biết tên giáo viên tham gia nhiều đề tài nhất của bô môn hệ thống thông tin
SELECT GV.HOTEN
FROM GIAOVIEN GV
JOIN THAMGIADT TG ON GV.MAGV = TG.MAGV
WHERE GV.MABM = 'HTTT'
GROUP BY GV.MAGV, GV.HOTEN
HAVING COUNT(TG.MADT) = (
	SELECT MAX(SLDTTG)
	FROM (SELECT COUNT(TG1.MADT) SLDTTG
		  FROM THAMGIADT TG1
		  JOIN GIAOVIEN GV1 ON TG1.MAGV = GV1.MAGV
		  WHERE GV1.MABM = 'HTTT'
		  GROUP BY GV1.MAGV) SLLN)

-- Q56: Cho biết tên giáo viên và tên bộ môn của giáo viên có nhiều người thân nhất
SELECT GV.HOTEN, BM.TENBM
FROM GIAOVIEN GV
JOIN BOMON BM ON GV.MABM = BM.MABM
JOIN NGUOITHAN NT ON GV.MAGV = NT.MAGV
GROUP BY GV.MAGV, GV.HOTEN, BM.TENBM
HAVING COUNT(NT.TEN) = (
	SELECT MAX(SLNT)
	FROM (SELECT COUNT(TEN) SLNT
		  FROM NGUOITHAN 
		  GROUP BY MAGV) SLLN)

-- Q57: Cho biết tên trưởng bộ môn mà chủ nhiệm nhiều đề tài nhất
SELECT TBM.HOTEN
FROM GIAOVIEN TBM
JOIN BOMON BM ON TBM.MABM = BM.MABM AND TBM.MAGV = BM.TRUONGBM
JOIN DETAI DT ON TBM.MAGV = DT.GVCNDT
GROUP BY TBM.MAGV, TBM.HOTEN
HAVING COUNT(DT.MADT) = (
	SELECT MAX(SLDT)
	FROM (SELECT COUNT(MADT) SLDT
	FROM DETAI
	GROUP BY GVCNDT) SLLN)







