import requests
from time import sleep, time
from enum import Enum
import datetime
import sys

DATE_FORMAT = "%Y-%m-%dT%H:%M:%S.%fZ"

def str_to_date(date:str):
	return datetime.datetime.strptime(date, DATE_FORMAT)

class Feedback(dict):
	def __init__(self, comment:str = "", rating: int = -1):
		dict.__init__(self, comment=comment, rating=rating)

	@property
	def comment(self)->str:
		return self["comment"]
	@comment.setter
	def comment(self, v):
		self["comment"] = v
	@property
	def rating(self)->int:
		return self["rating"]
	@rating.setter
	def rating(self, v):
		self["rating"] = v

class Cursus:
	def __init__(self, start:datetime.datetime, end:datetime.datetime, id:int):
		self.start = start
		self.end = end
		self.id = id


class Correction(dict):

	def __init__(self, comment:str = "", grade:int = -1, corrector:str = "",
			feedback:Feedback = Feedback(), cursus_id:int=-1, begin_at:datetime.datetime=None,
			filled_at:datetime.datetime=None):
		dict.__init__(self, cursus_id=cursus_id, comment=comment, grade=grade, corrector=corrector, feedback=feedback, begin_at=begin_at, filled_at=filled_at)

	@property
	def begin_at(self)->datetime.datetime:
		return self["begin_at"]
	@begin_at.setter
	def begin_at(self, v):
		self["begin_at"] = v
	@property
	def filled_at(self)->datetime.datetime:
		return self["filled_at"]
	@filled_at.setter
	def filled_at(self, v):
		self["filled_at"] = v
	@property
	def cursus_id(self)->int:
		return self["cursus_id"]
	@cursus_id.setter
	def cursus_id(self, v):
		self["cursus_id"] = v
	@property
	def comment(self)->str:
		return self["comment"]
	@comment.setter
	def comment(self, v):
		self["comment"] = v
	@property
	def grade(self)->int:
		return self["grade"]
	@grade.setter
	def grade(self, v):
		self["grade"] = v
	@property
	def corrector(self)->str:
		return self["corrector"]
	@corrector.setter
	def corrector(self, v):
		self["corrector"] = v
	@property
	def feedback(self)->Feedback:
		return self["feedback"]
	@feedback.setter
	def feedback(self, v):
		self["feedback"] = v

class APIIntra:
	def get_tokens(ids:list)->list:
		tokens = []
		for id in ids:
			try:
				x = requests.post("https://api.intra.42.fr/oauth/token",
						json={ "grant_type":"client_credentials", "client_id":id[0], "client_secret":id[1] }).json()
			except Exception as e:
				print("An error occured while retrieving the token:")
				print(e)
				continue
			if ("error" in x):
				print("An error occured while retrieving the token:")
				print(x)
				continue
			try:
				token = x["access_token"]
				tokens.append(token)
			except:
				print("An error occured while retrieving the token:")
				print(x)
				continue
		return (tokens)

	def __init__(self, ids:list):
		self.tokens = {x:0 for x in APIIntra.get_tokens(ids)}

	def make_request(self, method, *args, **kwargs):
		current_min = sys.maxsize
		tokens_count = len(self.tokens.keys())
		for token in self.tokens.keys():
			elapsed_time = time() - self.tokens[token]
			if (elapsed_time < 0.6 / tokens_count):
				if (elapsed_time < current_min):
					current_min = elapsed_time
				continue
			if (not kwargs.get("headers")):
				kwargs["headers"] = {}
			kwargs["headers"].update({"Authorization":"Bearer " + token})
			response = method(*args, **kwargs)
			self.tokens[token] = time()
			return response
		sleep((0.6/ tokens_count) - current_min)
		return self.make_request(method, *args, **kwargs)

		


def printProgressBar (iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
	"""
	Call in a loop to create terminal progress bar
	@params:
		iteration    - Required  : current iteration (Int)
		total        - Required  : total iterations (Int)
		prefix       - Optional  : prefix string (Str)
		suffix	     - Optional  : suffix string (Str)
		decimals	 - Optional  : positive number of decimals in percent complete (Int)
		length	     - Optional  : character length of bar (Int)
		fill         - Optional  : bar fill character (Str)
		printEnd     - Optional  : end character (e.g. "\r", "\r\n") (Str)
	"""
	percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
	filledLength = int(length * iteration // total)
	bar = fill * filledLength + '-' * (length - filledLength)
	print(f'\r{prefix} |{bar}| {percent}% {suffix}', end = printEnd)
	# Print New Line on Complete
	if iteration == total: 
		print()



def get_42mulhouse_users(apiintra:APIIntra, filters={})->list:
	total_users = []
	users = [1]
	params = {
		"filter[primary_campus_id]": 48,
		"filter[kind]": "student",
		"page[size]": 100
	}
	params.update(filters)
	page = 1
	while (len(users) != 0):
		params["page[number]"] = page
		x=apiintra.make_request(requests.get,
			"https://api.intra.42.fr/v2/users",
			params=params,
		)
		if (x.status_code != 200):
			raise Exception("Bad status code")
		users = x.json()
		for e in users:
			if (e["active?"]):
				total_users.append(e)
		page += 1
	return total_users


def get_full_user_info(apiintra:APIIntra, user_id):
	x=apiintra.make_request(requests.get, f"https://api.intra.42.fr/v2/users/{user_id}")
	return x.json()


def get_all_scales(apiintra:APIIntra, user_id: int):
	page = 1
	scales = [None]
	total_scales = []
	while (len(scales) != 0):
		x=apiintra.make_request(requests.get, f"https://api.intra.42.fr/v2/users/{user_id}/scale_teams/as_corrected",
		params={"page[size]":100, "page[number]":page})
		scales = x.json()
		total_scales += scales
		page += 1
	return total_scales
