<?php
$params = array();
parse_str($_SERVER['QUERY_STRING'], $params);
$login = $params['login'];
echo "<!DOCTYPE html>\r\n<html>\r\n<body>\r\n<h2>$login : Hello World!</h2>\r\n<h2>World : Hello You !</h2>\r\n</body>\r\n</html>";
?>