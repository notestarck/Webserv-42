#!/usr/bin/env python3
from api_intra import *
import json
import argparse
import csv
import datetime
import time
import numpy as np

def str_to_date(date:str):
	return datetime.datetime.strptime(date, DATE_FORMAT)

IDS=[
	("u-s4t2ud-4c150f08adb6bfe1fcec95f1e89c29c57466fb64a84190e8e848547974a1682e", "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"),
	("u-s4t2ud-977a6b148fea884d4f8ed7e1d8ea98abcd117007d50936fe4e35d81b4d1cdc54", "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"),
	("u-s4t2ud-de63cfed097ef2a22d0abd65c627740a633618c1625638badc61303ba6d9572e", "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")
]

if (__name__ == "__main__"):

	#============================== Parse arguments
	parser = argparse.ArgumentParser(prog="Cheater List", description="List the cheaters of 42 mulhouse")
	parser.add_argument("login", nargs='*', default=None, metavar="login")
	parser.add_argument("-u", "--update", nargs=1, dest="update", required=False, metavar="Update")
	parser.add_argument("-g", "--graph", nargs=1, dest="graph", required=False, metavar="Graphics")
	group = parser.add_argument_group("search infos")
	group.add_argument("-fn", "--first-name", nargs='*', dest="fn", required=False, metavar="First name")
	group.add_argument("-ln", "--last-name", nargs='*', dest="ln", required=False, metavar="Last name")
	args = parser.parse_args()
	filters = {}
	if (args.login != None):
		filters["filter[login]"] = ','.join(args.login)
	if (args.fn != None):
		filters["filter[first_name]"] = ','.join(args.fn)
	if (args.ln != None):
		filters["filter[last_name]"] = ','.join(args.ln)
	#==============================

	apiintra = APIIntra(IDS)


	users = get_42mulhouse_users(apiintra, filters=filters)

	user_corrections_total = {}
	corrections_tmp = {}
	users_len = len(users)
	start_time = time.time()
	all_times = np.array([])
	for user, i in zip(users, range(users_len)):

		elapsed_time = time.time() - start_time
		all_times = np.append(all_times, elapsed_time)
		average = np.average(all_times)
		start_time = time.time()

		user = get_full_user_info(apiintra, user["id"])

		all_cursus = []
		for cursus in user["cursus_users"]:
			start_date = str_to_date(cursus["begin_at"])
			end_date = str_to_date(cursus["end_at"]) if cursus["end_at"] else datetime.datetime.max
			id = cursus["cursus_id"]
			all_cursus.append(Cursus(start_date, end_date, id))

		all_scales = get_all_scales(apiintra, user["id"])
		corrections = [None] * len(all_scales)
		for i, entry in enumerate(all_scales):
			correction = Correction()
			correction.comment:str = entry["comment"]
			if correction.comment is None:
				correction.comment = ""
			correction.grade = entry["final_mark"]
			correction.corrector = entry["corrector"]["login"]
			correction.begin_at = str_to_date(entry["begin_at"])
			try:
				correction.filled_at = str_to_date(entry["filled_at"])
			except Exception as e: #CORRECTION ANNULE ??
				#print(entry)
				#print(e)
				continue
			try:
				fb = Feedback()
				fb.comment = entry["feedbacks"][0]["comment"]
				if fb.comment is None:
					fb.comment = ""
				fb.rating = entry["feedbacks"][0]["rating"]
				correction.feedback = fb
			except:
				pass
			for cursus in all_cursus:
				if (cursus.start < correction.begin_at <= cursus.end):
					correction.cursus_id = cursus.id
			corrections[i] = correction

		corrections_tmp[user["login"]] = corrections

		corrections_total = {c.corrector:{ "total":0, **{cursus.id:0 for cursus in all_cursus} } for c in corrections if c is not None}
		for c in corrections:
			if c is None:
				continue
			corrections_total[c.corrector][c.cursus_id] = (corrections_total[c.corrector][c.cursus_id] if corrections_total[c.corrector].get(c.cursus_id) is not None else 0) + 1
			corrections_total[c.corrector]["total"] += 1
		user_corrections_total[user["login"]] = corrections_total



	user_corrections_total_len = len(user_corrections_total.keys())







	for i, user in enumerate(user_corrections_total.keys()):
		sorted_dict = dict(sorted(user_corrections_total[user].items(), key=lambda it: it[1]['total'], reverse=True))
		for corrector in sorted_dict.keys():
			print(f"User '{user}' corrected by '{corrector}' : {sorted_dict[corrector]['total']}")
#			f.write(f"{user};")
#			f.write(f"{corrector};")
#			f.write(f"{user_corrections_total[user][corrector]['total']};")
#			for k in user_corrections_total[user][corrector].keys():
#				if (k == "total"):
#					continue
#				f.write(f"{k};")
#				f.write(f"{user_corrections_total[user][corrector][k]};")
#			f.write("\n")

