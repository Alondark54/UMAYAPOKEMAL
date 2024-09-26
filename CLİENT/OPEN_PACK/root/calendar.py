import sys
import datetime
import locale as _locale
from itertools import repeat

__all__ = ["IllegalMonthError", "IllegalWeekdayError", "setfirstweekday",
			"firstweekday", "isleap", "leapdays", "weekday", "monthrange",
			"monthcalendar", "prmonth", "month", "prcal", "calendar",
			"timegm", "month_name", "month_abbr", "day_name", "day_abbr",
			"Calendar", "TextCalendar", "HTMLCalendar", "LocaleTextCalendar",
			"LocaleHTMLCalendar", "weekheader",
			"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY",
			"SATURDAY", "SUNDAY"]

EventCalendar = {
	# 0	: ["Gün", 				"Tür",		"Drop",							MinLv-MaxLv,	"Event Ýsmi",			"Bþ.Saati-Bt.Saati",	Ýtem-Adedi],
	0	: ["SÜRPÝZ ETKÝNLÝK",			"EXP",		"Seviyene Göre Canavar",		1, 120,			"BALIK ADASI ETKÝNLÝÐÝ",		"XX:XX",	"XX:XX",	0,0],
	1	: ["SÜRPÝZ ETKÝNLÝK",			"EXP",		"Seviyene Göre Metin",			1, 120,			"AVCI ETKÝNLÝÐÝ",		"XX:XX",	"XX:XX",	0,0],
	2	: ["SÜRPÝZ ETKÝNLÝK",		"Nesne",	"Seviyene Göre Canavar",		1, 120,			"X2 DROP ETKÝNLÝÐÝ",		"XX:XX",	"XX:XX",	0,0],
	3	: ["SÜRPÝZ ETKÝNLÝK",			"Nesne",	"Seviyene Göre Canavar",		1, 120,			"KAOS EVENTÝ",			"XX:XX",	"XX:XX",	0,0],
	4	: ["SÜRPÝZ ETKÝNLÝK",		"Nesne",	"Seviyene Göre Canavar",		1, 120,			"BULMACA KUTUSU",		"XX:XX",	"XX:XX",	0,0],
	5	: ["SÜRPÝZ ETKÝNLÝK",		"Nesne",	"Seviyene Göre Metin",			1, 120,			"OKEY KART ETKÝNLÝÐÝ",		"XX:XX",	"XX:XX",	0,0],
	6	: ["SÜRPÝZ ETKÝNLÝK",			"Nesne",	"Seviyene Göre Canavar",	1, 120,			"METÝN TAÞI SANDIÐI",		"XX:XX",	"XX:XX",	0,0],
	7	: ["Cuma",			"Derece",	"Seviyene Göre Canavar",		1, 120,			"Çark Etkinliði",		"22:00",	"23:59",	0,0],
	8	: ["Cumartesi",		"Nesne",	"Metin ve Canavar",					1, 120,			"Futbol Topu",	"22:00",	"23:00",	0,0],
	9	: ["Pazar",		"Nesne",	"Metin ve Canavar",			1, 120,			"Altýgen Etkinliði",	"14:00",	"15:00",	0,0],
	10	: ["YAKINDA",		"Nesne",	"Metin ve Canavar",			1, 120,			"YAKINDA",	"14:00",	"15:00",	0,0],
	}

# Exception raised for bad input (with string parameter for details)
error = ValueError

# Exceptions raised for bad input
class IllegalMonthError(ValueError):
	def __init__(self, month):
		self.month = month
	def __str__(self):
		return "bad month number %r; must be 1-12" % self.month

class IllegalWeekdayError(ValueError):
	def __init__(self, weekday):
		self.weekday = weekday
	def __str__(self):
		return "bad weekday number %r; must be 0 (Monday) to 6 (Sunday)" % self.weekday


# Constants for months referenced later
January = 1
February = 2

# Number of days per month (except for February in leap years)
mdays = [0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

# This module used to have hard-coded lists of day and month names, as
# English strings.  The classes following emulate a read-only version of
# that, but supply localized names.  Note that the values are computed
# fresh on each call, in case the user changes locale between calls.

class _localized_month:

	_months = [datetime.date(2001, i+1, 1).strftime for i in range(12)]
	_months.insert(0, lambda x: "")

	def __init__(self, format):
		self.format = format

	def __getitem__(self, i):
		funcs = self._months[i]
		if isinstance(i, slice):
			return [f(self.format) for f in funcs]
		else:
			return funcs(self.format)

	def __len__(self):
		return 13

class _localized_day:

	# January 1, 2001, was a Monday.
	_days = [datetime.date(2001, 1, i+1).strftime for i in range(7)]

	def __init__(self, format):
		self.format = format

	def __getitem__(self, i):
		funcs = self._days[i]
		if isinstance(i, slice):
			return [f(self.format) for f in funcs]
		else:
			return funcs(self.format)

	def __len__(self):
		return 7

# Full and abbreviated names of weekdays
day_name = _localized_day('%A')
day_abbr = _localized_day('%a')

# Full and abbreviated names of months (1-based arrays!!!)
month_name = _localized_month('%B')
month_abbr = _localized_month('%b')

# Constants for weekdays
(MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY) = range(7)

def isleap(year):
	return year % 4 == 0 and (year % 100 != 0 or year % 400 == 0)

def leapdays(y1, y2):
	y1 -= 1
	y2 -= 1
	return (y2//4 - y1//4) - (y2//100 - y1//100) + (y2//400 - y1//400)

def weekday(year, month, day):
	if not datetime.MINYEAR <= year <= datetime.MAXYEAR:
		year = 2000 + year % 400
	return datetime.date(year, month, day).weekday()

def monthrange(year, month):
	if not 1 <= month <= 12:
		raise IllegalMonthError(month)
	day1 = weekday(year, month, 1)
	ndays = mdays[month] + (month == February and isleap(year))
	return day1, ndays

def _monthlen(year, month):
	return mdays[month] + (month == February and isleap(year))

def _prevmonth(year, month):
	if month == 1:
		return year-1, 12
	else:
		return year, month-1

def _nextmonth(year, month):
	if month == 12:
		return year+1, 1
	else:
		return year, month+1

if __name__ == "__main__":
	main(sys.argv)
