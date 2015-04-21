<?php
	
	
	function _paywhirl_create_user($paywhirldata) {
		
		if ($existing = user_load(array('mail' => $paywhirldata["email"]))) {
		
			$account['paywhirl_id'] = $paywhirldata["stripe_id"];
			$account['paywhirl_password'] = md5("" . $paywhirldata["password"]);
		
			$existing = user_save($existing, $account);
			$password = "";
			return $existing;
		}
		else {
			// Only set a user name if we are not updating an existing record.
			if (strpos($paywhirldata["email"], "@") > 1) {
				$username = drupal_substr($paywhirldata["email"], 0, strpos($paywhirldata["email"], "@"));
			} else {
				$username = $paywhirldata["first_name"] . $paywhirldata["last_name"];
			}
			$account = array();
			$account['name'] = _paywhirl_create_username(trim($username));
			$account['mail'] = $paywhirldata["email"];
			$account['profile_name'] = $paywhirldata["first_name"] . " " . $paywhirldata["last_name"];
			$account['pass'] = $paywhirldata["password"];
			$account['access'] = 1;
			$account['status'] = 1;			
			$account['paywhirl_id'] = $paywhirldata["stripe_id"];
			$account['paywhirl_password'] = md5("" . $paywhirldata["password"]);
			
			$profile = json_decode($data["profile"], true);
			foreach (array_expression as $key => $value) {
				$account["profile_".strtolower($key)] = $value;
			}
			
			
			$account = user_save('', $account);
			return $account;
		}
	}
	
	
	
	function _paywhirl_load_user($email, $password) {
	
		
		$datasearch = '%s:17:"paywhirl_password";s:32:"'.md5($password).'";%';
		$uid = db_result(db_query_range('SELECT uid FROM {users} WHERE mail = "%s" AND data like \'%s\'', $email, $datasearch  , 0, 1));
		
		if (isset($uid)) {
			if ($existing = user_load($uid)) {
				return $existing;
			}
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
