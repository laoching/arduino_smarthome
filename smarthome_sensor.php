<?php
header("Content-Type: text/html;charset=UTF-8");				//웹페이지 코딩 html기반, 유니코드 인코딩 방식 utf-8로 선언
 
$host = 'localhost';							//db 주소 = localhost
$user = 'root';								//db user이름 = root
$pw = 'root';								//db password = root
$dbName = 'testdb';							//db 이름 = testdb
$mysqli = new mysqli($host, $user, $pw, $dbName);				//db와 연결하는 쿼리문(함수의 인자는 host, db id, db password, db name이다.)
    if($mysqli){								//db가 연결되면
	echo "MySQL successfully connected!<br/>";				//MySQL successfully connected! 문자를 띄운다

	$temp = $_GET['temp'];						//db의 temp column에 아두이노에 선언된 temp변수 값을 넣는다
	$humi = $_GET['humi'];						//db의 humi column에 아두이노에 선언된 humi변수 값을 넣는다
	$dust = $_GET['dust'];						//db의 dust column에 아두이노에 선언된 dust변수 값을 넣는다
	$flame = $_GET['flame'];						//db의 flame column에 아두이노에 선언된 flame변수 값을 넣는다	
	$light = $_GET['light'];						//db의 light column에 아두이노에 선언된 light변수 값을 넣는다
	$cnt = $_GET['cnt'];						//db의 cnt column에 아두이노에 선언된 cnt변수 값을 넣는다

	//testdb에 있는 temphumi 테이블에 column순서대로 측정된 변수 값을 넣는 $query라는 쿼리문 선언
	$query = "INSERT INTO temphumi (temp, humi, dust, flame, light, cnt) VALUES ('$temp','$humi','$dust','$flame','$light','$cnt')";
	mysqli_query($mysqli,$query);					//mysql에 $query문 실행



	echo "</br>success!!";						//success 출력
    }

    else{									//MySQL 연결되지 않으면
	echo "MySQL could not be connected";				//MySQL could not be connected 출력
    }
	echo $row['$temp'];						//웹페이지에 db의 temp열을 출력한다
	echo $row['$humi'];						//웹페이지에 db의 humi열을 출력한다
	echo $row['$dust'];						//웹페이지에 db의 dust열을 출력한다
	echo $row['$flame'];						//웹페이지에 db의 flame열을 출력한다
	echo $row['$light'];						//웹페이지에 db의 light열을 출력한다
mysqli_close($mysqli);							//mysql 종료
?>