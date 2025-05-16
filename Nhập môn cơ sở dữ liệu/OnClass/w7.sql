USE QLDT
GO

-- SELECT ==> Muốn lấy thuộc tính --> TƯƠNG QUAN
-- FROM ==> Tạo bảng mới --> Tính toán giá trị để xuất/ Viết điều kiện/ Gom các thuộc tính --> PHÂN CẤP/TƯƠNG QUAN
-- WHERE ==> So sánh thuộc tính trong bảng --> TƯƠNG QUAN
-- HAVING ==> So sánh kết quả của hàng kết hợp theo SUM, MAX, MIN, AVG,...


-- Lượng từ:
---- All:
---- Any:
---- Some

---- Truy vấn lồng phân cấp ----
---- Sub-query không truy xuất thuộc tính cha
---- VD: Cho biết giáo viên không có người thân


---- VD: Cho biết giáo viên và số lượng đề tài chủ nhiệm
SELECT GV.*, DT.SL
FROM GIAOVIEN GV JOIN (
	SELECT GVCNDT, COUNT(*) SL
	FROM DETAI DT
	GROUP BY DT.GVCNDT) DT
ON GV.MAGV = DT.GVCNDT







---- Truy vấn lồng tương quan
---- VD: Cho biết giáo viên không có người thân
SELECT *
FROM GIAOVIEN GV
WHERE NOT EXISTS (SELECT *
				  FROM NGUOITHAN NT
				  WHERE NT.MAGV = GV.MAGV)

---- VD: Cho biết giáo viên và số lượng đề tài chủ nhiệm
SELECT GV.*, (SELECT COUNT(*) SL
			  FROM DETAI DT
			  WHERE DT.GVCNDT = GV.MAGV)
FROM GIAOVIEN GV



---- VD: Cho biết giáo viên chủ nhiệm nhiều đề tài nhất
SELECT GV.*
FROM DETAI DT
JOIN GIAOVIEN GV ON DT.GVCNDT = GV.MAGV
GROUP BY DT.GVCNDT
HAVING COUNT(DT.MADT) >= all(SELECT COUNT(MADT)
						  FROM DETAI
						  GROUP BY GVCNDT)



SELECT GV.MAGV, GV.HOTEN
FROM DETAI DT
JOIN GIAOVIEN GV ON DT.GVCNDT = GV.MAGV
GROUP BY DT.GVCNDT, GV.MAGV, GV.HOTEN
HAVING COUNT(DT.MADT) = (SELECT MAX(SL)
						 FROM (SELECT COUNT(MADT) SL
							   FROM DETAI
							   GROUP BY GVCNDT) SLLN
					    )


---- VD: Cho biết giáo viên cùng tên & cùng giới tính với giáo viên khác trong cùng bộ môn
SELECT GV.*
FROM GIAOVIEN GV JOIN (SELECT MAGV, HOTEN, PHAI, MABM
					   FROM GIAOVIEN) GV1
ON GV1.HOTEN = GV.HOTEN AND GV1.MABM = GV.MABM AND GV1.PHAI = GV.PHAI AND GV1.MAGV <> GV.MAGV



SELECT GV.*
FROM GIAOVIEN GV
WHERE GV.HOTEN IN (
	SELECT GV1.HOTEN
	FROM GIAOVIEN GV1
	WHERE GV1.MABM = GV.MABM AND GV1.PHAI = GV.PHAI AND GV1.MAGV <> GV.MAGV)

-- VD: Cho biết giáo viên có lương lớn nhất
SELECT GV.*
FROM GIAOVIEN GV
WHERE GV.LUONG = (SELECT MAX(LUONG)
				  FROM GIAOVIEN)






---- VD: Cho biết giáo viên tham gia đề tài nhiều nhất trong từng bộ môn
SELECT TG.MAGV, GV.MABM, COUNT(DISTINCT MADT) SLTG
FROM THAMGIADT TG
JOIN GIAOVIEN GV ON TG.MAGV = GV.MAGV
GROUP BY TG.MAGV, GV.MABM
HAVING COUNT(DISTINCT MADT) >= ALL (SELECT COUNT(DISTINCT MADT)
									FROM THAMGIADT TG1
									JOIN GIAOVIEN GV1 ON TG1.MAGV = GV1.MAGV
									WHERE GV1.MABM = GV.MABM
									GROUP BY TG1.MAGV)