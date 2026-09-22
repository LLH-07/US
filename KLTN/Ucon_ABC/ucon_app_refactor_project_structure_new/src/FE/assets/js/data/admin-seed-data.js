// Dữ liệu Admin mô phỏng theo BE/src/main/resources/data.sql.
// Dùng để show bảng giống phpMyAdmin/DBeaver mini trong Admin.
// Chỉ ảnh hưởng Admin UI; luồng đọc User vẫn đi qua PEP/CH backend.
window.ADMIN_DEMO_DB = {
  "USERS": [
    {
      "id": "1",
      "name": "Nguyễn Thị Bình",
      "email": "nguyenbinh123@gmail.com",
      "hashed_password": "demo",
      "status": "ACTIVE"
    },
    {
      "id": "2",
      "name": "Trần Minh An",
      "email": "an.demo@gmail.com",
      "hashed_password": "demo",
      "status": "ACTIVE"
    },
    {
      "id": "3",
      "name": "Lê Bảo Châu",
      "email": "chau.demo@gmail.com",
      "hashed_password": "demo",
      "status": "ACTIVE"
    },
    {
      "id": "4",
      "name": "Hà Linh",
      "email": "halinh.demo@gmail.com",
      "hashed_password": "demo",
      "status": "ACTIVE"
    },
    {
      "id": "5",
      "name": "Phan Thanh Tùng",
      "email": "ptt.demo@gmail.com",
      "hashed_password": "demo",
      "status": "ACTIVE"
    },
    {
      "id": "6",
      "name": "Đỗ Gia Hân",
      "email": "han.demo@gmail.com",
      "hashed_password": "demo",
      "status": "ACTIVE"
    },
    {
      "id": "7",
      "name": "Phạm Hoàng Nam",
      "email": "nam.demo@gmail.com",
      "hashed_password": "demo",
      "status": "ACTIVE"
    },
    {
      "id": "8",
      "name": "Võ Minh Khang",
      "email": "khang.demo@gmail.com",
      "hashed_password": "demo",
      "status": "ACTIVE"
    }
  ],
  "BOOKS": [
    {
      "id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối",
      "genre": "mystery",
      "authors": "Karen M. McManus",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "2",
      "title": "Nghệ Thuật Nói Trước Công Chúng",
      "genre": "communication",
      "authors": "Dale Carnegie",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "3",
      "title": "Dám Hạnh Phúc",
      "genre": "psychology",
      "authors": "Kishimi Ichiro",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "4",
      "title": "Những Kẻ Xuất Chúng",
      "genre": "non-fiction",
      "authors": "MalCom Gladwell",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "5",
      "title": "Vận Hành Tinh Gọn",
      "genre": "business",
      "authors": "Ash Maurya",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "6",
      "title": "Sherlock Holmes Toàn Tập",
      "genre": "classic-detective",
      "authors": "Arthur Conan Doyle",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "7",
      "title": "Ghi Chép Pháp Y 4: Lật Lại Hồ Sơ Tội Ác",
      "genre": "true-crime",
      "authors": "Lưu Bát Bách",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "8",
      "title": "Ba Người Lính Ngự Lâm",
      "genre": "classic",
      "authors": "Alexandre Dumas",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "9",
      "title": "Dưới Bầu Trời Trắng - Tương Lai Của Tự Nhiên Và Con Người Trong Kỷ Nguyên Biến Đổi Khí Hậu",
      "genre": "science",
      "authors": "Hà Hường",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "10",
      "title": "Đất Xanh Sinh Cú",
      "genre": "fiction",
      "authors": "Vĩ Ngư",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "11",
      "title": "The Flames",
      "genre": "classic-scifi",
      "authors": "Olaf Stapledon",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "12",
      "title": "Biggles of the Camel Squadron",
      "genre": "classic-adventure",
      "authors": "E. W. Johns",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "13",
      "title": "The Green Eagle",
      "genre": "classic-adventure",
      "authors": "Lester Dent",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "14",
      "title": "The Red Pony",
      "genre": "literature",
      "authors": "John Steinbeck",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "15",
      "title": "In Our Time",
      "genre": "literature",
      "authors": "Ernest Hemingway",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "16",
      "title": "The Dinner Club",
      "genre": "classic-mystery",
      "authors": "Sapper",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "17",
      "title": "The Castaways of the Flag",
      "genre": "classic-adventure",
      "authors": "Jules Verne",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "18",
      "title": "Five Fall into Adventure",
      "genre": "adventure",
      "authors": "Enid Blyton",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "19",
      "title": "Dám Nghĩ Lại",
      "genre": "psychology",
      "authors": "Adam Grant",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "20",
      "title": "Of Mice and Men",
      "genre": "literature",
      "authors": "John Steinbeck",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "21",
      "title": "White Nights and Other Stories",
      "genre": "classic",
      "authors": "Fyodor Dostoyevsky",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "22",
      "title": "Wuthering Heights",
      "genre": "classic",
      "authors": "Emily Brontë",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "23",
      "title": "Frankenstein",
      "genre": "classic-horror",
      "authors": "Mary Shelley",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "24",
      "title": "The Great Gatsby",
      "genre": "classic",
      "authors": "F. Scott Fitzgerald",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    },
    {
      "id": "25",
      "title": "The Bell Jar",
      "genre": "literature",
      "authors": "Sylvia Plath",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "26",
      "title": "Youth and the Bright Medusa",
      "genre": "classic",
      "authors": "Willa Cather",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "27",
      "title": "Divine Comedy: Purgatory",
      "genre": "classic",
      "authors": "Dante Alighieri",
      "default_access_model": "RENTAL",
      "status": "ACTIVE"
    },
    {
      "id": "28",
      "title": "The True Story of My Life",
      "genre": "memoir",
      "authors": "Hans Christian Andersen",
      "default_access_model": "FREE",
      "status": "ACTIVE"
    },
    {
      "id": "29",
      "title": "Casanova Episode 1: Venetian Years",
      "genre": "memoir",
      "authors": "Giacomo Casanova",
      "default_access_model": "PAID",
      "status": "ACTIVE"
    }
  ],
  "BOOK_VOLUMES": [
    {
      "id": "1001",
      "book_id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "1002",
      "book_id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "1003",
      "book_id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "1004",
      "book_id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "1005",
      "book_id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối - Tập 05",
      "volume_no": "5",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "1006",
      "book_id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối - Tập 06",
      "volume_no": "6",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "1007",
      "book_id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối - Tập 07",
      "volume_no": "7",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "1008",
      "book_id": "1",
      "title": "Trong Chúng Tôi Có Kẻ Nói Dối - Tập 08",
      "volume_no": "8",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "2001",
      "book_id": "2",
      "title": "Nghệ Thuật Nói Trước Công Chúng - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "2002",
      "book_id": "2",
      "title": "Nghệ Thuật Nói Trước Công Chúng - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "2003",
      "book_id": "2",
      "title": "Nghệ Thuật Nói Trước Công Chúng - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "2004",
      "book_id": "2",
      "title": "Nghệ Thuật Nói Trước Công Chúng - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "2005",
      "book_id": "2",
      "title": "Nghệ Thuật Nói Trước Công Chúng - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "2006",
      "book_id": "2",
      "title": "Nghệ Thuật Nói Trước Công Chúng - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "3001",
      "book_id": "3",
      "title": "Dám Hạnh Phúc - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "3002",
      "book_id": "3",
      "title": "Dám Hạnh Phúc - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "3003",
      "book_id": "3",
      "title": "Dám Hạnh Phúc - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "3004",
      "book_id": "3",
      "title": "Dám Hạnh Phúc - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "3005",
      "book_id": "3",
      "title": "Dám Hạnh Phúc - Tập 05",
      "volume_no": "5",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "4001",
      "book_id": "4",
      "title": "Những Kẻ Xuất Chúng - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "4002",
      "book_id": "4",
      "title": "Những Kẻ Xuất Chúng - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "4003",
      "book_id": "4",
      "title": "Những Kẻ Xuất Chúng - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "4004",
      "book_id": "4",
      "title": "Những Kẻ Xuất Chúng - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "4005",
      "book_id": "4",
      "title": "Những Kẻ Xuất Chúng - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "4006",
      "book_id": "4",
      "title": "Những Kẻ Xuất Chúng - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "4007",
      "book_id": "4",
      "title": "Những Kẻ Xuất Chúng - Tập 07",
      "volume_no": "7",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "5001",
      "book_id": "5",
      "title": "Vận Hành Tinh Gọn - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "5002",
      "book_id": "5",
      "title": "Vận Hành Tinh Gọn - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "5003",
      "book_id": "5",
      "title": "Vận Hành Tinh Gọn - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "5004",
      "book_id": "5",
      "title": "Vận Hành Tinh Gọn - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "5005",
      "book_id": "5",
      "title": "Vận Hành Tinh Gọn - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6001",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6002",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6003",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6004",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6005",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6006",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6007",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 07",
      "volume_no": "7",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6008",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 08",
      "volume_no": "8",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6009",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 09",
      "volume_no": "9",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6010",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 10",
      "volume_no": "10",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6011",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 11",
      "volume_no": "11",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6012",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 12",
      "volume_no": "12",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6013",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 13",
      "volume_no": "13",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6014",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 14",
      "volume_no": "14",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6015",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 15",
      "volume_no": "15",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6016",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 16",
      "volume_no": "16",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6017",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 17",
      "volume_no": "17",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6018",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 18",
      "volume_no": "18",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6019",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 19",
      "volume_no": "19",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6020",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 20",
      "volume_no": "20",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6021",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 21",
      "volume_no": "21",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6022",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 22",
      "volume_no": "22",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6023",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 23",
      "volume_no": "23",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6024",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 24",
      "volume_no": "24",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6025",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 25",
      "volume_no": "25",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6026",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 26",
      "volume_no": "26",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6027",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 27",
      "volume_no": "27",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6028",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 28",
      "volume_no": "28",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6029",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 29",
      "volume_no": "29",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6030",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 30",
      "volume_no": "30",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6031",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 31",
      "volume_no": "31",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6032",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 32",
      "volume_no": "32",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6033",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 33",
      "volume_no": "33",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6034",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 34",
      "volume_no": "34",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6035",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 35",
      "volume_no": "35",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6036",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 36",
      "volume_no": "36",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6037",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 37",
      "volume_no": "37",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6038",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 38",
      "volume_no": "38",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6039",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 39",
      "volume_no": "39",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6040",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 40",
      "volume_no": "40",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6041",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 41",
      "volume_no": "41",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6042",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 42",
      "volume_no": "42",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6043",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 43",
      "volume_no": "43",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6044",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 44",
      "volume_no": "44",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6045",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 45",
      "volume_no": "45",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6046",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 46",
      "volume_no": "46",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6047",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 47",
      "volume_no": "47",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6048",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 48",
      "volume_no": "48",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6049",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 49",
      "volume_no": "49",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6050",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 50",
      "volume_no": "50",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6051",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 51",
      "volume_no": "51",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6052",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 52",
      "volume_no": "52",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6053",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 53",
      "volume_no": "53",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "6054",
      "book_id": "6",
      "title": "Sherlock Holmes Toàn Tập - Tập 54",
      "volume_no": "54",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "7001",
      "book_id": "7",
      "title": "Ghi Chép Pháp Y 4: Lật Lại Hồ Sơ Tội Ác - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "7002",
      "book_id": "7",
      "title": "Ghi Chép Pháp Y 4: Lật Lại Hồ Sơ Tội Ác - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "7003",
      "book_id": "7",
      "title": "Ghi Chép Pháp Y 4: Lật Lại Hồ Sơ Tội Ác - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "7004",
      "book_id": "7",
      "title": "Ghi Chép Pháp Y 4: Lật Lại Hồ Sơ Tội Ác - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8001",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8002",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8003",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8004",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8005",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8006",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8007",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 07",
      "volume_no": "7",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8008",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 08",
      "volume_no": "8",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8009",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 09",
      "volume_no": "9",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8010",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 10",
      "volume_no": "10",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8011",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 11",
      "volume_no": "11",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8012",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 12",
      "volume_no": "12",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8013",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 13",
      "volume_no": "13",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8014",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 14",
      "volume_no": "14",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8015",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 15",
      "volume_no": "15",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8016",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 16",
      "volume_no": "16",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8017",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 17",
      "volume_no": "17",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8018",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 18",
      "volume_no": "18",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "8019",
      "book_id": "8",
      "title": "Ba Người Lính Ngự Lâm - Tập 19",
      "volume_no": "19",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "9001",
      "book_id": "9",
      "title": "Dưới Bầu Trời Trắng - Tương Lai Của Tự Nhiên Và Con Người Trong Kỷ Nguyên Biến Đổi Khí Hậu - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "9002",
      "book_id": "9",
      "title": "Dưới Bầu Trời Trắng - Tương Lai Của Tự Nhiên Và Con Người Trong Kỷ Nguyên Biến Đổi Khí Hậu - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "9003",
      "book_id": "9",
      "title": "Dưới Bầu Trời Trắng - Tương Lai Của Tự Nhiên Và Con Người Trong Kỷ Nguyên Biến Đổi Khí Hậu - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "9004",
      "book_id": "9",
      "title": "Dưới Bầu Trời Trắng - Tương Lai Của Tự Nhiên Và Con Người Trong Kỷ Nguyên Biến Đổi Khí Hậu - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "9005",
      "book_id": "9",
      "title": "Dưới Bầu Trời Trắng - Tương Lai Của Tự Nhiên Và Con Người Trong Kỷ Nguyên Biến Đổi Khí Hậu - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "9006",
      "book_id": "9",
      "title": "Dưới Bầu Trời Trắng - Tương Lai Của Tự Nhiên Và Con Người Trong Kỷ Nguyên Biến Đổi Khí Hậu - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10001",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10002",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10003",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10004",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10005",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 05",
      "volume_no": "5",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10006",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 06",
      "volume_no": "6",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10007",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 07",
      "volume_no": "7",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10008",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 08",
      "volume_no": "8",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10009",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 09",
      "volume_no": "9",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10010",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 10",
      "volume_no": "10",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10011",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 11",
      "volume_no": "11",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10012",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 12",
      "volume_no": "12",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10013",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 13",
      "volume_no": "13",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10014",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 14",
      "volume_no": "14",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10015",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 15",
      "volume_no": "15",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10016",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 16",
      "volume_no": "16",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10017",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 17",
      "volume_no": "17",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10018",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 18",
      "volume_no": "18",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10019",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 19",
      "volume_no": "19",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10020",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 20",
      "volume_no": "20",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10021",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 21",
      "volume_no": "21",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10022",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 22",
      "volume_no": "22",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10023",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 23",
      "volume_no": "23",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10024",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 24",
      "volume_no": "24",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10025",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 25",
      "volume_no": "25",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10026",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 26",
      "volume_no": "26",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10027",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 27",
      "volume_no": "27",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10028",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 28",
      "volume_no": "28",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10029",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 29",
      "volume_no": "29",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10030",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 30",
      "volume_no": "30",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10031",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 31",
      "volume_no": "31",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10032",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 32",
      "volume_no": "32",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10033",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 33",
      "volume_no": "33",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "10034",
      "book_id": "10",
      "title": "Đất Xanh Sinh Cú - Tập 34",
      "volume_no": "34",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "11001",
      "book_id": "11",
      "title": "The Flames - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "11002",
      "book_id": "11",
      "title": "The Flames - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "11003",
      "book_id": "11",
      "title": "The Flames - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "12001",
      "book_id": "12",
      "title": "Biggles of the Camel Squadron - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "12002",
      "book_id": "12",
      "title": "Biggles of the Camel Squadron - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "12003",
      "book_id": "12",
      "title": "Biggles of the Camel Squadron - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "12004",
      "book_id": "12",
      "title": "Biggles of the Camel Squadron - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "13001",
      "book_id": "13",
      "title": "The Green Eagle - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "13002",
      "book_id": "13",
      "title": "The Green Eagle - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "13003",
      "book_id": "13",
      "title": "The Green Eagle - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "13004",
      "book_id": "13",
      "title": "The Green Eagle - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "14001",
      "book_id": "14",
      "title": "The Red Pony - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "14002",
      "book_id": "14",
      "title": "The Red Pony - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "14003",
      "book_id": "14",
      "title": "The Red Pony - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "15001",
      "book_id": "15",
      "title": "In Our Time - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "16001",
      "book_id": "16",
      "title": "The Dinner Club - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "16002",
      "book_id": "16",
      "title": "The Dinner Club - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "16003",
      "book_id": "16",
      "title": "The Dinner Club - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "16004",
      "book_id": "16",
      "title": "The Dinner Club - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "16005",
      "book_id": "16",
      "title": "The Dinner Club - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "16006",
      "book_id": "16",
      "title": "The Dinner Club - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "17001",
      "book_id": "17",
      "title": "The Castaways of the Flag - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "17002",
      "book_id": "17",
      "title": "The Castaways of the Flag - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "17003",
      "book_id": "17",
      "title": "The Castaways of the Flag - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "17004",
      "book_id": "17",
      "title": "The Castaways of the Flag - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "17005",
      "book_id": "17",
      "title": "The Castaways of the Flag - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "18001",
      "book_id": "18",
      "title": "Five Fall into Adventure - Tập 01",
      "volume_no": "1",
      "price": "79000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "18002",
      "book_id": "18",
      "title": "Five Fall into Adventure - Tập 02",
      "volume_no": "2",
      "price": "79000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "18003",
      "book_id": "18",
      "title": "Five Fall into Adventure - Tập 03",
      "volume_no": "3",
      "price": "79000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "18004",
      "book_id": "18",
      "title": "Five Fall into Adventure - Tập 04",
      "volume_no": "4",
      "price": "79000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "19001",
      "book_id": "19",
      "title": "Dám Nghĩ Lại - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "19002",
      "book_id": "19",
      "title": "Dám Nghĩ Lại - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "19003",
      "book_id": "19",
      "title": "Dám Nghĩ Lại - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "19004",
      "book_id": "19",
      "title": "Dám Nghĩ Lại - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "19005",
      "book_id": "19",
      "title": "Dám Nghĩ Lại - Tập 05",
      "volume_no": "5",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "19006",
      "book_id": "19",
      "title": "Dám Nghĩ Lại - Tập 06",
      "volume_no": "6",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "19007",
      "book_id": "19",
      "title": "Dám Nghĩ Lại - Tập 07",
      "volume_no": "7",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "19008",
      "book_id": "19",
      "title": "Dám Nghĩ Lại - Tập 08",
      "volume_no": "8",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "20001",
      "book_id": "20",
      "title": "Of Mice and Men - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "20002",
      "book_id": "20",
      "title": "Of Mice and Men - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "20003",
      "book_id": "20",
      "title": "Of Mice and Men - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21001",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21002",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21003",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21004",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21005",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 05",
      "volume_no": "5",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21006",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 06",
      "volume_no": "6",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21007",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 07",
      "volume_no": "7",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21008",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 08",
      "volume_no": "8",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21009",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 09",
      "volume_no": "9",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "21010",
      "book_id": "21",
      "title": "White Nights and Other Stories - Tập 10",
      "volume_no": "10",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22001",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22002",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22003",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22004",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22005",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22006",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22007",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 07",
      "volume_no": "7",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22008",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 08",
      "volume_no": "8",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22009",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 09",
      "volume_no": "9",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "22010",
      "book_id": "22",
      "title": "Wuthering Heights - Tập 10",
      "volume_no": "10",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "23001",
      "book_id": "23",
      "title": "Frankenstein - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "23002",
      "book_id": "23",
      "title": "Frankenstein - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "23003",
      "book_id": "23",
      "title": "Frankenstein - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "23004",
      "book_id": "23",
      "title": "Frankenstein - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "23005",
      "book_id": "23",
      "title": "Frankenstein - Tập 05",
      "volume_no": "5",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "23006",
      "book_id": "23",
      "title": "Frankenstein - Tập 06",
      "volume_no": "6",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "23007",
      "book_id": "23",
      "title": "Frankenstein - Tập 07",
      "volume_no": "7",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "24001",
      "book_id": "24",
      "title": "The Great Gatsby - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "24002",
      "book_id": "24",
      "title": "The Great Gatsby - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "24003",
      "book_id": "24",
      "title": "The Great Gatsby - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "24004",
      "book_id": "24",
      "title": "The Great Gatsby - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "25001",
      "book_id": "25",
      "title": "The Bell Jar - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "25002",
      "book_id": "25",
      "title": "The Bell Jar - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "25003",
      "book_id": "25",
      "title": "The Bell Jar - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "25004",
      "book_id": "25",
      "title": "The Bell Jar - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "25005",
      "book_id": "25",
      "title": "The Bell Jar - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "25006",
      "book_id": "25",
      "title": "The Bell Jar - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "26001",
      "book_id": "26",
      "title": "Youth and the Bright Medusa - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "26002",
      "book_id": "26",
      "title": "Youth and the Bright Medusa - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "26003",
      "book_id": "26",
      "title": "Youth and the Bright Medusa - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "26004",
      "book_id": "26",
      "title": "Youth and the Bright Medusa - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "26005",
      "book_id": "26",
      "title": "Youth and the Bright Medusa - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "26006",
      "book_id": "26",
      "title": "Youth and the Bright Medusa - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "27001",
      "book_id": "27",
      "title": "Divine Comedy: Purgatory - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "27002",
      "book_id": "27",
      "title": "Divine Comedy: Purgatory - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "27003",
      "book_id": "27",
      "title": "Divine Comedy: Purgatory - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "RENTAL",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "28001",
      "book_id": "28",
      "title": "The True Story of My Life - Tập 01",
      "volume_no": "1",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "28002",
      "book_id": "28",
      "title": "The True Story of My Life - Tập 02",
      "volume_no": "2",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "28003",
      "book_id": "28",
      "title": "The True Story of My Life - Tập 03",
      "volume_no": "3",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "28004",
      "book_id": "28",
      "title": "The True Story of My Life - Tập 04",
      "volume_no": "4",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "28005",
      "book_id": "28",
      "title": "The True Story of My Life - Tập 05",
      "volume_no": "5",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "28006",
      "book_id": "28",
      "title": "The True Story of My Life - Tập 06",
      "volume_no": "6",
      "price": "0",
      "access_model_override": "FREE",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "29001",
      "book_id": "29",
      "title": "Casanova Episode 1: Venetian Years - Tập 01",
      "volume_no": "1",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "29002",
      "book_id": "29",
      "title": "Casanova Episode 1: Venetian Years - Tập 02",
      "volume_no": "2",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "29003",
      "book_id": "29",
      "title": "Casanova Episode 1: Venetian Years - Tập 03",
      "volume_no": "3",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "29004",
      "book_id": "29",
      "title": "Casanova Episode 1: Venetian Years - Tập 04",
      "volume_no": "4",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "29005",
      "book_id": "29",
      "title": "Casanova Episode 1: Venetian Years - Tập 05",
      "volume_no": "5",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    },
    {
      "id": "29006",
      "book_id": "29",
      "title": "Casanova Episode 1: Venetian Years - Tập 06",
      "volume_no": "6",
      "price": "99000",
      "access_model_override": "PAID",
      "status": "ACTIVE",
      "read_count": ""
    }
  ],
  "RENTAL_PLANS": [
    {
      "id": "1",
      "name": "Gói tháng",
      "duration_days": "30",
      "price": "59000"
    },
    {
      "id": "2",
      "name": "Gói năm",
      "duration_days": "365",
      "price": "499000"
    }
  ],
  "USER_RENTALS": [
    {
      "id": "1",
      "user_id": "1",
      "plan_id": "1",
      "started_at": "now() - interval '2 days'",
      "expires_at": "now() + interval '28 days'",
      "status": "ACTIVE"
    },
    {
      "id": "2",
      "user_id": "4",
      "plan_id": "2",
      "started_at": "now() - interval '10 days'",
      "expires_at": "now() + interval '355 days'",
      "status": "ACTIVE"
    },
    {
      "id": "3",
      "user_id": "5",
      "plan_id": "1",
      "started_at": "now() - interval '45 days'",
      "expires_at": "now() - interval '15 days'",
      "status": "ACTIVE"
    }
  ],
  "VOLUME_PURCHASES": [
    {
      "id": "1",
      "user_id": "1",
      "volume_id": "1001",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "2",
      "user_id": "1",
      "volume_id": "1002",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "3",
      "user_id": "1",
      "volume_id": "1003",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "4",
      "user_id": "1",
      "volume_id": "1004",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "5",
      "user_id": "1",
      "volume_id": "1005",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "6",
      "user_id": "1",
      "volume_id": "1006",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "7",
      "user_id": "1",
      "volume_id": "1007",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "8",
      "user_id": "1",
      "volume_id": "1008",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "9",
      "user_id": "1",
      "volume_id": "3001",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "10",
      "user_id": "1",
      "volume_id": "3002",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "11",
      "user_id": "1",
      "volume_id": "3003",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "12",
      "user_id": "1",
      "volume_id": "3004",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "13",
      "user_id": "1",
      "volume_id": "3005",
      "price": "99000",
      "purchased_at": "now() - interval '8 days'"
    },
    {
      "id": "14",
      "user_id": "2",
      "volume_id": "7001",
      "price": "99000",
      "purchased_at": "now() - interval '9 days'"
    },
    {
      "id": "15",
      "user_id": "2",
      "volume_id": "7002",
      "price": "99000",
      "purchased_at": "now() - interval '9 days'"
    },
    {
      "id": "16",
      "user_id": "2",
      "volume_id": "7003",
      "price": "99000",
      "purchased_at": "now() - interval '9 days'"
    },
    {
      "id": "17",
      "user_id": "2",
      "volume_id": "7004",
      "price": "99000",
      "purchased_at": "now() - interval '9 days'"
    },
    {
      "id": "18",
      "user_id": "4",
      "volume_id": "10001",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "19",
      "user_id": "4",
      "volume_id": "10002",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "20",
      "user_id": "4",
      "volume_id": "10003",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "21",
      "user_id": "4",
      "volume_id": "10004",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "22",
      "user_id": "4",
      "volume_id": "10005",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "23",
      "user_id": "4",
      "volume_id": "10006",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "24",
      "user_id": "4",
      "volume_id": "10007",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "25",
      "user_id": "4",
      "volume_id": "10008",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "26",
      "user_id": "4",
      "volume_id": "10009",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "27",
      "user_id": "4",
      "volume_id": "10010",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "28",
      "user_id": "4",
      "volume_id": "10011",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "29",
      "user_id": "4",
      "volume_id": "10012",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "30",
      "user_id": "4",
      "volume_id": "10013",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "31",
      "user_id": "4",
      "volume_id": "10014",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "32",
      "user_id": "4",
      "volume_id": "10015",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "33",
      "user_id": "4",
      "volume_id": "10016",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "34",
      "user_id": "4",
      "volume_id": "10017",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "35",
      "user_id": "4",
      "volume_id": "10018",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "36",
      "user_id": "4",
      "volume_id": "10019",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "37",
      "user_id": "4",
      "volume_id": "10020",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "38",
      "user_id": "4",
      "volume_id": "10021",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "39",
      "user_id": "4",
      "volume_id": "10022",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "40",
      "user_id": "4",
      "volume_id": "10023",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "41",
      "user_id": "4",
      "volume_id": "10024",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "42",
      "user_id": "4",
      "volume_id": "10025",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "43",
      "user_id": "4",
      "volume_id": "10026",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "44",
      "user_id": "4",
      "volume_id": "10027",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "45",
      "user_id": "4",
      "volume_id": "10028",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "46",
      "user_id": "4",
      "volume_id": "10029",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "47",
      "user_id": "4",
      "volume_id": "10030",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "48",
      "user_id": "4",
      "volume_id": "10031",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "49",
      "user_id": "4",
      "volume_id": "10032",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "50",
      "user_id": "4",
      "volume_id": "10033",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "51",
      "user_id": "4",
      "volume_id": "10034",
      "price": "99000",
      "purchased_at": "now() - interval '11 days'"
    },
    {
      "id": "52",
      "user_id": "5",
      "volume_id": "18001",
      "price": "79000",
      "purchased_at": "now() - interval '12 days'"
    },
    {
      "id": "53",
      "user_id": "5",
      "volume_id": "18002",
      "price": "79000",
      "purchased_at": "now() - interval '12 days'"
    },
    {
      "id": "54",
      "user_id": "5",
      "volume_id": "18003",
      "price": "79000",
      "purchased_at": "now() - interval '12 days'"
    },
    {
      "id": "55",
      "user_id": "5",
      "volume_id": "18004",
      "price": "79000",
      "purchased_at": "now() - interval '12 days'"
    }
  ],
  "FAMILY_GROUPS": [
    {
      "id": "1",
      "owner_user_id": "1",
      "name": "Gia đình nhà Bình",
      "status": "ACTIVE"
    },
    {
      "id": "2",
      "owner_user_id": "2",
      "name": "Nhóm tri thức",
      "status": "ACTIVE"
    },
    {
      "id": "3",
      "owner_user_id": "1",
      "name": "Tủ sách cuối tuần",
      "status": "ACTIVE"
    },
    {
      "id": "4",
      "owner_user_id": "4",
      "name": "Câu lạc bộ đọc sách",
      "status": "ACTIVE"
    },
    {
      "id": "5",
      "owner_user_id": "6",
      "name": "Kho sách học tập",
      "status": "ACTIVE"
    }
  ],
  "FAMILY_GROUP_MEMBERS": [
    {
      "id": "1",
      "group_id": "1",
      "user_id": "1",
      "status": "ACTIVE",
      "joined_at": "now() - interval '10 days'",
      "removed_at": ""
    },
    {
      "id": "2",
      "group_id": "1",
      "user_id": "2",
      "status": "ACTIVE",
      "joined_at": "now() - interval '9 days'",
      "removed_at": ""
    },
    {
      "id": "3",
      "group_id": "1",
      "user_id": "5",
      "status": "ACTIVE",
      "joined_at": "now() - interval '8 days'",
      "removed_at": ""
    },
    {
      "id": "4",
      "group_id": "2",
      "user_id": "2",
      "status": "ACTIVE",
      "joined_at": "now() - interval '7 days'",
      "removed_at": ""
    },
    {
      "id": "5",
      "group_id": "2",
      "user_id": "1",
      "status": "ACTIVE",
      "joined_at": "now() - interval '7 days'",
      "removed_at": ""
    },
    {
      "id": "6",
      "group_id": "2",
      "user_id": "4",
      "status": "ACTIVE",
      "joined_at": "now() - interval '6 days'",
      "removed_at": ""
    },
    {
      "id": "7",
      "group_id": "3",
      "user_id": "1",
      "status": "ACTIVE",
      "joined_at": "now() - interval '5 days'",
      "removed_at": ""
    },
    {
      "id": "8",
      "group_id": "3",
      "user_id": "5",
      "status": "ACTIVE",
      "joined_at": "now() - interval '5 days'",
      "removed_at": ""
    },
    {
      "id": "9",
      "group_id": "4",
      "user_id": "4",
      "status": "ACTIVE",
      "joined_at": "now() - interval '4 days'",
      "removed_at": ""
    },
    {
      "id": "10",
      "group_id": "4",
      "user_id": "3",
      "status": "ACTIVE",
      "joined_at": "now() - interval '4 days'",
      "removed_at": ""
    },
    {
      "id": "11",
      "group_id": "5",
      "user_id": "6",
      "status": "ACTIVE",
      "joined_at": "now() - interval '3 days'",
      "removed_at": ""
    },
    {
      "id": "12",
      "group_id": "5",
      "user_id": "7",
      "status": "ACTIVE",
      "joined_at": "now() - interval '2 days'",
      "removed_at": ""
    }
  ],
  "FAMILY_SHARED_VOLUMES": [
    {
      "id": "1",
      "group_id": "1",
      "volume_id": "1001",
      "status": "ACTIVE",
      "shared_at": "now() - interval '3 days'",
      "revoked_at": ""
    },
    {
      "id": "2",
      "group_id": "1",
      "volume_id": "1002",
      "status": "ACTIVE",
      "shared_at": "now() - interval '3 days'",
      "revoked_at": ""
    },
    {
      "id": "3",
      "group_id": "1",
      "volume_id": "1003",
      "status": "ACTIVE",
      "shared_at": "now() - interval '3 days'",
      "revoked_at": ""
    },
    {
      "id": "4",
      "group_id": "1",
      "volume_id": "1004",
      "status": "ACTIVE",
      "shared_at": "now() - interval '3 days'",
      "revoked_at": ""
    },
    {
      "id": "5",
      "group_id": "1",
      "volume_id": "1005",
      "status": "ACTIVE",
      "shared_at": "now() - interval '3 days'",
      "revoked_at": ""
    },
    {
      "id": "6",
      "group_id": "1",
      "volume_id": "1006",
      "status": "ACTIVE",
      "shared_at": "now() - interval '3 days'",
      "revoked_at": ""
    },
    {
      "id": "7",
      "group_id": "1",
      "volume_id": "1007",
      "status": "ACTIVE",
      "shared_at": "now() - interval '3 days'",
      "revoked_at": ""
    },
    {
      "id": "8",
      "group_id": "1",
      "volume_id": "1008",
      "status": "ACTIVE",
      "shared_at": "now() - interval '3 days'",
      "revoked_at": ""
    },
    {
      "id": "9",
      "group_id": "2",
      "volume_id": "7001",
      "status": "ACTIVE",
      "shared_at": "now() - interval '4 days'",
      "revoked_at": ""
    },
    {
      "id": "10",
      "group_id": "2",
      "volume_id": "7002",
      "status": "ACTIVE",
      "shared_at": "now() - interval '4 days'",
      "revoked_at": ""
    },
    {
      "id": "11",
      "group_id": "2",
      "volume_id": "7003",
      "status": "ACTIVE",
      "shared_at": "now() - interval '4 days'",
      "revoked_at": ""
    },
    {
      "id": "12",
      "group_id": "2",
      "volume_id": "7004",
      "status": "ACTIVE",
      "shared_at": "now() - interval '4 days'",
      "revoked_at": ""
    },
    {
      "id": "13",
      "group_id": "3",
      "volume_id": "3001",
      "status": "ACTIVE",
      "shared_at": "now() - interval '5 days'",
      "revoked_at": ""
    },
    {
      "id": "14",
      "group_id": "3",
      "volume_id": "3002",
      "status": "ACTIVE",
      "shared_at": "now() - interval '5 days'",
      "revoked_at": ""
    },
    {
      "id": "15",
      "group_id": "3",
      "volume_id": "3003",
      "status": "ACTIVE",
      "shared_at": "now() - interval '5 days'",
      "revoked_at": ""
    },
    {
      "id": "16",
      "group_id": "3",
      "volume_id": "3004",
      "status": "ACTIVE",
      "shared_at": "now() - interval '5 days'",
      "revoked_at": ""
    },
    {
      "id": "17",
      "group_id": "3",
      "volume_id": "3005",
      "status": "ACTIVE",
      "shared_at": "now() - interval '5 days'",
      "revoked_at": ""
    },
    {
      "id": "18",
      "group_id": "4",
      "volume_id": "10001",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "19",
      "group_id": "4",
      "volume_id": "10002",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "20",
      "group_id": "4",
      "volume_id": "10003",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "21",
      "group_id": "4",
      "volume_id": "10004",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "22",
      "group_id": "4",
      "volume_id": "10005",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "23",
      "group_id": "4",
      "volume_id": "10006",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "24",
      "group_id": "4",
      "volume_id": "10007",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "25",
      "group_id": "4",
      "volume_id": "10008",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "26",
      "group_id": "4",
      "volume_id": "10009",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "27",
      "group_id": "4",
      "volume_id": "10010",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "28",
      "group_id": "4",
      "volume_id": "10011",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "29",
      "group_id": "4",
      "volume_id": "10012",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "30",
      "group_id": "4",
      "volume_id": "10013",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "31",
      "group_id": "4",
      "volume_id": "10014",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "32",
      "group_id": "4",
      "volume_id": "10015",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "33",
      "group_id": "4",
      "volume_id": "10016",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "34",
      "group_id": "4",
      "volume_id": "10017",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "35",
      "group_id": "4",
      "volume_id": "10018",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "36",
      "group_id": "4",
      "volume_id": "10019",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "37",
      "group_id": "4",
      "volume_id": "10020",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "38",
      "group_id": "4",
      "volume_id": "10021",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "39",
      "group_id": "4",
      "volume_id": "10022",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "40",
      "group_id": "4",
      "volume_id": "10023",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "41",
      "group_id": "4",
      "volume_id": "10024",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "42",
      "group_id": "4",
      "volume_id": "10025",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "43",
      "group_id": "4",
      "volume_id": "10026",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "44",
      "group_id": "4",
      "volume_id": "10027",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "45",
      "group_id": "4",
      "volume_id": "10028",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "46",
      "group_id": "4",
      "volume_id": "10029",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "47",
      "group_id": "4",
      "volume_id": "10030",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "48",
      "group_id": "4",
      "volume_id": "10031",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "49",
      "group_id": "4",
      "volume_id": "10032",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "50",
      "group_id": "4",
      "volume_id": "10033",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    },
    {
      "id": "51",
      "group_id": "4",
      "volume_id": "10034",
      "status": "ACTIVE",
      "shared_at": "now() - interval '6 days'",
      "revoked_at": ""
    }
  ],
  "SYSTEM_PARAMETERS": [
    {
      "id": "1",
      "parameter_name": "MAX_FAMILY_SHARED_USERS_PER_VOLUME",
      "value": "2",
      "unit": "users"
    },
    {
      "id": "2",
      "parameter_name": "BOOK_CONTENT_CHUNK_SIZE",
      "value": "3400",
      "unit": "characters"
    },
    {
      "id": "3",
      "parameter_name": "BOOK_VOLUME_PAGE_COUNT",
      "value": "20",
      "unit": "chunks_per_volume"
    },
    {
      "id": "4",
      "parameter_name": "UCON_ADMIN_USER_ID",
      "value": "1",
      "unit": "user_id"
    },
    {
      "id": "101",
      "parameter_name": "BOOK_1_CONTENT_FILE",
      "value": "ebooks/01-trong-chung-toi-co-ke-noi-doi-karen-m-mcmanus-truong-trung-tin-dich.epub",
      "unit": "classpath"
    },
    {
      "id": "102",
      "parameter_name": "BOOK_2_CONTENT_FILE",
      "value": "ebooks/02-nghe-thuat-noi-truoc-cong-chung-dale-carnegie.epub",
      "unit": "classpath"
    },
    {
      "id": "103",
      "parameter_name": "BOOK_3_CONTENT_FILE",
      "value": "ebooks/03-dam-hanh-phuc-kishimi-ichiro-koga-fumitake.epub",
      "unit": "classpath"
    },
    {
      "id": "104",
      "parameter_name": "BOOK_4_CONTENT_FILE",
      "value": "ebooks/04-nhung-ke-xuat-chung-malcolm-gladwell.epub",
      "unit": "classpath"
    },
    {
      "id": "105",
      "parameter_name": "BOOK_5_CONTENT_FILE",
      "value": "ebooks/05-van-hanh-tinh-gon-ash-maurya.epub",
      "unit": "classpath"
    },
    {
      "id": "106",
      "parameter_name": "BOOK_6_CONTENT_FILE",
      "value": "ebooks/06-sherlock-holmes-toan-tap-arthur-conan-doyle.epub",
      "unit": "classpath"
    },
    {
      "id": "107",
      "parameter_name": "BOOK_7_CONTENT_FILE",
      "value": "ebooks/07-ghi-chep-phap-y-4-lat-lai-ho-so-toi-ac-luu-bat-bach-linh-tu-dich.epub",
      "unit": "classpath"
    },
    {
      "id": "108",
      "parameter_name": "BOOK_8_CONTENT_FILE",
      "value": "ebooks/08-ba-nguoi-linh-ngu-lam-alexandre-dumas.epub",
      "unit": "classpath"
    },
    {
      "id": "109",
      "parameter_name": "BOOK_9_CONTENT_FILE",
      "value": "ebooks/09-duoi-bau-troi-trang-tuong-lai-cua-tu-nhien-va-con-nguoi-trongy-nguyen.epub",
      "unit": "classpath"
    },
    {
      "id": "110",
      "parameter_name": "BOOK_10_CONTENT_FILE",
      "value": "ebooks/10-dat-xanh-sinh-cu-vi-ngu-ai-dich.epub",
      "unit": "classpath"
    },
    {
      "id": "111",
      "parameter_name": "BOOK_11_CONTENT_FILE",
      "value": "ebooks/11-stapledon-flames.epub",
      "unit": "classpath"
    },
    {
      "id": "112",
      "parameter_name": "BOOK_12_CONTENT_FILE",
      "value": "ebooks/12-johns-biggles-of-the-camel-squadron.epub",
      "unit": "classpath"
    },
    {
      "id": "113",
      "parameter_name": "BOOK_13_CONTENT_FILE",
      "value": "ebooks/13-dent-green-eagle.epub",
      "unit": "classpath"
    },
    {
      "id": "114",
      "parameter_name": "BOOK_14_CONTENT_FILE",
      "value": "ebooks/14-steinbeck-red-pony.epub",
      "unit": "classpath"
    },
    {
      "id": "115",
      "parameter_name": "BOOK_15_CONTENT_FILE",
      "value": "ebooks/15-hemingway-in-our-time.epub",
      "unit": "classpath"
    },
    {
      "id": "116",
      "parameter_name": "BOOK_16_CONTENT_FILE",
      "value": "ebooks/16-mcneile-dinner-club.epub",
      "unit": "classpath"
    },
    {
      "id": "117",
      "parameter_name": "BOOK_17_CONTENT_FILE",
      "value": "ebooks/17-verne-castaways-of-the-flag.epub",
      "unit": "classpath"
    },
    {
      "id": "118",
      "parameter_name": "BOOK_18_CONTENT_FILE",
      "value": "ebooks/18-blyton-five-fall-into-adventure.epub",
      "unit": "classpath"
    },
    {
      "id": "119",
      "parameter_name": "BOOK_19_CONTENT_FILE",
      "value": "ebooks/19-dam-nghi-lai-adam-grant.epub",
      "unit": "classpath"
    },
    {
      "id": "120",
      "parameter_name": "BOOK_20_CONTENT_FILE",
      "value": "ebooks/20-steinbeck-of-mice-and-men.epub",
      "unit": "classpath"
    },
    {
      "id": "121",
      "parameter_name": "BOOK_21_CONTENT_FILE",
      "value": "ebooks/21-dostoyevsky-white-nights-and-other-stories.epub",
      "unit": "classpath"
    },
    {
      "id": "122",
      "parameter_name": "BOOK_22_CONTENT_FILE",
      "value": "ebooks/22-bronte-wuthering-heights.epub",
      "unit": "classpath"
    },
    {
      "id": "123",
      "parameter_name": "BOOK_23_CONTENT_FILE",
      "value": "ebooks/23-shelley-frankenstein.epub",
      "unit": "classpath"
    },
    {
      "id": "124",
      "parameter_name": "BOOK_24_CONTENT_FILE",
      "value": "ebooks/24-fitzgerald-great-gatsby.epub",
      "unit": "classpath"
    },
    {
      "id": "125",
      "parameter_name": "BOOK_25_CONTENT_FILE",
      "value": "ebooks/25-plath-bell-jar.epub",
      "unit": "classpath"
    },
    {
      "id": "126",
      "parameter_name": "BOOK_26_CONTENT_FILE",
      "value": "ebooks/26-cather-youth-and-the-bright-medusa.epub",
      "unit": "classpath"
    },
    {
      "id": "127",
      "parameter_name": "BOOK_27_CONTENT_FILE",
      "value": "ebooks/27-alighieri-divine-comedy-purgatory.epub",
      "unit": "classpath"
    },
    {
      "id": "128",
      "parameter_name": "BOOK_28_CONTENT_FILE",
      "value": "ebooks/28-andersen-true-story-of-my-life.epub",
      "unit": "classpath"
    },
    {
      "id": "129",
      "parameter_name": "BOOK_29_CONTENT_FILE",
      "value": "ebooks/29-casanova-episode-1-venetian-years.epub",
      "unit": "classpath"
    }
  ],
  "POLICY_REGISTRY": [
    {
      "id": "1",
      "policy_ref": "rental_read_pre_policy",
      "policy_name": "PRE policy for RENTAL reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "RENTAL",
      "phase": "PRE",
      "enabled": "true",
      "description": "PRE decision for RENTAL read path"
    },
    {
      "id": "2",
      "policy_ref": "rental_read_ongoing_policy",
      "policy_name": "ONGOING policy for RENTAL reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "RENTAL",
      "phase": "ONGOING",
      "enabled": "true",
      "description": "ONGOING decision for RENTAL read path"
    },
    {
      "id": "3",
      "policy_ref": "rental_read_post_policy",
      "policy_name": "POST policy for RENTAL reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "RENTAL",
      "phase": "POST",
      "enabled": "true",
      "description": "POST decision for RENTAL read path"
    },
    {
      "id": "4",
      "policy_ref": "preview_read_pre_policy",
      "policy_name": "PRE policy for PREVIEW reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "PREVIEW",
      "phase": "PRE",
      "enabled": "true",
      "description": "PRE decision for PREVIEW read path"
    },
    {
      "id": "5",
      "policy_ref": "preview_read_ongoing_policy",
      "policy_name": "ONGOING policy for PREVIEW reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "PREVIEW",
      "phase": "ONGOING",
      "enabled": "true",
      "description": "ONGOING decision for PREVIEW read path"
    },
    {
      "id": "6",
      "policy_ref": "preview_read_post_policy",
      "policy_name": "POST policy for PREVIEW reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "PREVIEW",
      "phase": "POST",
      "enabled": "true",
      "description": "POST decision for PREVIEW read path"
    },
    {
      "id": "7",
      "policy_ref": "free_read_pre_policy",
      "policy_name": "PRE policy for FREE reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "FREE",
      "phase": "PRE",
      "enabled": "true",
      "description": "PRE decision for FREE read path"
    },
    {
      "id": "8",
      "policy_ref": "free_read_ongoing_policy",
      "policy_name": "ONGOING policy for FREE reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "FREE",
      "phase": "ONGOING",
      "enabled": "true",
      "description": "ONGOING decision for FREE read path"
    },
    {
      "id": "9",
      "policy_ref": "free_read_post_policy",
      "policy_name": "POST policy for FREE reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "FREE",
      "phase": "POST",
      "enabled": "true",
      "description": "POST decision for FREE read path"
    },
    {
      "id": "10",
      "policy_ref": "paid_read_pre_policy",
      "policy_name": "PRE policy for PAID reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "PAID",
      "phase": "PRE",
      "enabled": "true",
      "description": "PRE decision for PAID read path"
    },
    {
      "id": "11",
      "policy_ref": "paid_read_ongoing_policy",
      "policy_name": "ONGOING policy for PAID reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "PAID",
      "phase": "ONGOING",
      "enabled": "true",
      "description": "ONGOING decision for PAID read path"
    },
    {
      "id": "12",
      "policy_ref": "paid_read_post_policy",
      "policy_name": "POST policy for PAID reading",
      "resource_type": "BOOK_VOLUME",
      "action_id": "READ",
      "access_path": "PAID",
      "phase": "POST",
      "enabled": "true",
      "description": "POST decision for PAID read path"
    }
  ],
  "PIP_ATTRIBUTE_MAPPINGS": [
    {
      "attr_key": "subject.status",
      "xacml_attribute_id": "urn:ucon:attr:subject:status",
      "xacml_category": "urn:oasis:names:tc:xacml:1.0:subject-category:access-subject",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "UserAttributeReader",
      "dependency_name": "UserDependencyResolver",
      "monitor_type": "DB_EVENT",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "book.status",
      "xacml_attribute_id": "urn:ucon:attr:book:status",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:resource",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "EbookResourceAttributeReader",
      "dependency_name": "EbookResourceDependencyResolver",
      "monitor_type": "DB_EVENT",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "resource.status",
      "xacml_attribute_id": "urn:ucon:attr:resource:status",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:resource",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "EbookResourceAttributeReader",
      "dependency_name": "EbookResourceDependencyResolver",
      "monitor_type": "DB_EVENT",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "resource.effective_access_model",
      "xacml_attribute_id": "urn:ucon:attr:resource:effective_access_model",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:resource",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "EbookResourceAttributeReader",
      "dependency_name": "EbookResourceDependencyResolver",
      "monitor_type": "DB_EVENT",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "currentRental.active",
      "xacml_attribute_id": "urn:ucon:attr:currentRental:active",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:environment",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#boolean",
      "reader_name": "RentalAttributeReader",
      "dependency_name": "RentalDependencyResolver",
      "monitor_type": "HYBRID",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "currentRental.status",
      "xacml_attribute_id": "urn:ucon:attr:currentRental:status",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:environment",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "RentalAttributeReader",
      "dependency_name": "RentalDependencyResolver",
      "monitor_type": "DB_EVENT",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "currentRental.expires_at",
      "xacml_attribute_id": "urn:ucon:attr:currentRental:expires_at",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:environment",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#dateTime",
      "reader_name": "RentalAttributeReader",
      "dependency_name": "RentalDependencyResolver",
      "monitor_type": "TIMER",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "ucon.phase",
      "xacml_attribute_id": "urn:ucon:attr:phase",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:environment",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "RequestAttributeReader",
      "dependency_name": "",
      "monitor_type": "NONE",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "access.path",
      "xacml_attribute_id": "urn:ucon:attr:access:path",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:resource",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "RequestAttributeReader",
      "dependency_name": "",
      "monitor_type": "NONE",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "action.id",
      "xacml_attribute_id": "urn:ucon:attr:action:id",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:action",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "RequestAttributeReader",
      "dependency_name": "",
      "monitor_type": "NONE",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "resource.type",
      "xacml_attribute_id": "urn:ucon:attr:resource:type",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:resource",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#string",
      "reader_name": "RequestAttributeReader",
      "dependency_name": "",
      "monitor_type": "NONE",
      "mutable": "false",
      "enabled": "true"
    },
    {
      "attr_key": "resource.read_count",
      "xacml_attribute_id": "urn:ucon:attr:resource:read_count",
      "xacml_category": "urn:oasis:names:tc:xacml:3.0:attribute-category:resource",
      "xacml_data_type": "http://www.w3.org/2001/XMLSchema#integer",
      "reader_name": "EbookResourceAttributeReader",
      "dependency_name": "",
      "monitor_type": "NONE",
      "mutable": "true",
      "enabled": "true"
    }
  ],
  "PIP_UPDATE_ACTIONS": [
    {
      "action_code": "INCREASE_BOOK_VOLUME_READ_COUNT",
      "attr_key": "resource.read_count",
      "handler_name": "BookVolumeUsageCounterHandler",
      "operation": "INCREMENT",
      "default_value": "1",
      "enabled": "true"
    }
  ],
  "POLICY_ATTRIBUTE_REQUIREMENTS": [
    {
      "id": "1",
      "policy_id": "1",
      "attr_key": "subject.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "2",
      "policy_id": "1",
      "attr_key": "book.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "3",
      "policy_id": "1",
      "attr_key": "resource.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "4",
      "policy_id": "1",
      "attr_key": "resource.effective_access_model",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "5",
      "policy_id": "1",
      "attr_key": "currentRental.active",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "6",
      "policy_id": "1",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "7",
      "policy_id": "1",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "8",
      "policy_id": "1",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "9",
      "policy_id": "1",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "10",
      "policy_id": "2",
      "attr_key": "subject.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "11",
      "policy_id": "2",
      "attr_key": "book.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "12",
      "policy_id": "2",
      "attr_key": "resource.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "13",
      "policy_id": "2",
      "attr_key": "resource.effective_access_model",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "14",
      "policy_id": "2",
      "attr_key": "currentRental.active",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "15",
      "policy_id": "2",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "16",
      "policy_id": "2",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "17",
      "policy_id": "2",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "18",
      "policy_id": "2",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "19",
      "policy_id": "3",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "20",
      "policy_id": "3",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "21",
      "policy_id": "3",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "22",
      "policy_id": "3",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "23",
      "policy_id": "4",
      "attr_key": "subject.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "24",
      "policy_id": "4",
      "attr_key": "book.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "25",
      "policy_id": "4",
      "attr_key": "resource.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "26",
      "policy_id": "4",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "27",
      "policy_id": "4",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "28",
      "policy_id": "4",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "29",
      "policy_id": "4",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "30",
      "policy_id": "5",
      "attr_key": "subject.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "31",
      "policy_id": "5",
      "attr_key": "book.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "32",
      "policy_id": "5",
      "attr_key": "resource.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "33",
      "policy_id": "5",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "34",
      "policy_id": "5",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "35",
      "policy_id": "5",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "36",
      "policy_id": "5",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "37",
      "policy_id": "6",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "38",
      "policy_id": "6",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "39",
      "policy_id": "6",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "40",
      "policy_id": "6",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "41",
      "policy_id": "7",
      "attr_key": "subject.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "42",
      "policy_id": "7",
      "attr_key": "book.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "43",
      "policy_id": "7",
      "attr_key": "resource.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "44",
      "policy_id": "7",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "45",
      "policy_id": "7",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "46",
      "policy_id": "7",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "47",
      "policy_id": "7",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "48",
      "policy_id": "8",
      "attr_key": "subject.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "49",
      "policy_id": "8",
      "attr_key": "book.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "50",
      "policy_id": "8",
      "attr_key": "resource.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "51",
      "policy_id": "8",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "52",
      "policy_id": "8",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "53",
      "policy_id": "8",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "54",
      "policy_id": "8",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "55",
      "policy_id": "9",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "56",
      "policy_id": "9",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "57",
      "policy_id": "9",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "58",
      "policy_id": "9",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "59",
      "policy_id": "10",
      "attr_key": "subject.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "60",
      "policy_id": "10",
      "attr_key": "book.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "61",
      "policy_id": "10",
      "attr_key": "resource.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "62",
      "policy_id": "10",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "63",
      "policy_id": "10",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "64",
      "policy_id": "10",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "65",
      "policy_id": "10",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "66",
      "policy_id": "11",
      "attr_key": "subject.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "67",
      "policy_id": "11",
      "attr_key": "book.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "68",
      "policy_id": "11",
      "attr_key": "resource.status",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "69",
      "policy_id": "11",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "70",
      "policy_id": "11",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "71",
      "policy_id": "11",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "72",
      "policy_id": "11",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "73",
      "policy_id": "12",
      "attr_key": "ucon.phase",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "74",
      "policy_id": "12",
      "attr_key": "access.path",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "75",
      "policy_id": "12",
      "attr_key": "action.id",
      "usage_type": "EVALUATION",
      "required": "true"
    },
    {
      "id": "76",
      "policy_id": "12",
      "attr_key": "resource.type",
      "usage_type": "EVALUATION",
      "required": "true"
    }
  ],
  "USAGE_SESSIONS": [],
  "USAGE_SESSION_POLICIES": [],
  "USAGE_SESSION_DEPENDENCIES": [],
  "UCON_TRIGGER_EVENTS": [],
  "SESSION_EVENTS": [],
  "OBLIGATION_EXECUTIONS": []
};
