<?php
	
	function _paywhirl_load_user($email, $password) {
	
		$mail_uid = _paywhirl_existing_uid($email);
		
		echo "Mail: " . $mail_uid."\n";
		
		
		
		
		$uid = db_result(db_query_range('SELECT f.name, v.value FROM {profile_fields} f INNER JOIN {profile_values} v ON f.fid = v.fid WHERE f.name = "%s" AND uid = %d AND v.value = "%s"', "paywhirl_password", $mail_uid, md5($password), 0, 1));
	
		die("Final: " . $uid);
	
		$login_args = array('mail' => $data["email"], 'paywhirl_password' => md5($data["password"]));
		
			print_r($login_args);
		if ($existing = user_load($login_args)) {
			$user = $existing;
			print_r($user);
		}
	
		$query = db_query("select `uid` from {table_name} where `profile_fbuid` = %d ",$fbuserid );
		while($result = db_fetch_object($query)){
		 $uid = $result->uid ; 
		 $user_exists = user_load(array('uid'=>$uid));
		}	
	}
	
	
	/**
		*  conform to Drupal username rules
	*/
	function _paywhirl_sanitise_username($username) {
		
		// username cannot contain an illegal character
		$username = preg_replace('/[^a-zA-Z0-9@ ]/', ' ', $username);
		// username cannot contain multiple spaces in a row
		$username = preg_replace('/[ ]+/', ' ', $username);
		
		// username must be less than 56 characters
		$username = drupal_substr($username, 0, 56);
		
		// username cannot begin or end with a space
		$username = trim($username);
		return $username;
	}
	
	/**
		*  deal with duplicate usernames
	*/
	function _paywhirl_unique_username($username) {
		
		static $suffix = 1;
		
		if ($suffix < 2) {
			$duplicate = db_result(db_query_range("SELECT uid from {users} where name = '%s'", $username, 0, 1));
		}
		else {
			$usern = $username .''. $suffix;
			$duplicate = db_result(db_query_range("SELECT uid from {users} where name = '%s'", $usern, 0, 1));
		}
		
		// loop until name is valid
		if (!empty($duplicate)) {
			$suffix++;
			_paywhirl_unique_username($username);
		}
		
		// add number at end of username if it already exists
		$username = ($suffix < 2) ? $username : "$username$suffix";
		return $username;
	}
	
	function _paywhirl_create_username($username = "") {
		
		
		if (empty($username)) $username = _paywhirl_random_username();
		
		$username = _paywhirl_sanitise_username($username);
		$username = _paywhirl_unique_username($username);
		return $username;
	}
	
	function _paywhirl_random_username() {
		
		
		$vowels = 'aoueiy';
		$consonants = 'bcdfghjklmnpqrstvwxz';
		$length = 8;
		
		mt_srand((double) microtime() * 10000000);
		$next_vowel = 0;
		
		for ($count = 0; $count <= $length; $count++) {
			
			if ($next_vowel) {
				$rand = mt_rand(0, 5);
				$username .= $vowels{$rand};
				$next_vowel = 0;
				
			}
			else {
				$rand = mt_rand(0, 19);
				$username .= $consonants{$rand};
				$next_vowel = 1;
			}
		}
		
		return $username;
	}
	
	/**
	* Return an existing user ID, if present, for a given email.
	*/
	function _paywhirl_existing_uid($email) {
		static $uid;
		if (!isset($uid[$email])) {
			$uid[$email] = db_result(db_query_range("SELECT uid from {users} where mail= '%s'", $email, 0, 1));
		}
		return $uid[$email];
	}
