import datetime
import time


class Transaction(object):
    def __init__(self, dateTime, amt, channel, behavior):
        self.dateTime = dateTime
        self.amt = amt
        self.channel = channel
        self.behavior = behavior


class Rule(object):
    def __init__(self, amtThresh=0., timesThresh=1):
        self.amtThresh = amtThresh
        self.timesThresh = timesThresh

    def Run(self, txList, dateTimeStart):
        count = 0
        for i, tx in enumerate(txList):
            if (tx.dateTime >= dateTimeStart) and (tx.amt >= self.amtThresh):
                # print("index %d, date time: %d, amount: %f, channel: %s" % (i+1, tx.dateTime, tx.amt, tx.channel))
                count += 1

        if (count >= self.timesThresh):
            return True
        else:
            return False


def PyDateTime2C(dateTime):
    """
    param dateTime: python datetime object
    return: dateTime in int format as YYYYmmddHHMMDDSS for input parameter of c++ basefunction
    """
    return int(dateTime.strftime("%Y%m%d%H%M%S"))


def CDateTime2Py(dateTimeInInt):
    """
    param dateTimeInInt: dateTime in int format as YYYYmmddHHMMDDSS
    return: python datetime object
    """
    return datetime.datetime.strptime(str(dateTimeInInt), "%Y%m%d%H%M%S")


if __name__ == "__main__":
    rule1 = Rule(
        amtThresh=10.0,
        timesThresh=2
    )
    rule2 = Rule(
        amtThresh=100.0
    )
    rule3 = Rule(
        amtThresh=10.0,
        timesThresh=1
    )
    ruleList = [rule1, rule2, rule3]
    dateTimeStartList = [
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(days=-7)),
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(days=-2)),
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(hours=-1)),
    ]

    tx1 = Transaction(
        PyDateTime2C(datetime.datetime.now()),
        10,
        "IBMB".encode('utf-8'),
        "轉入".encode('utf-8')
    )
    tx2 = Transaction(
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(days=-2, hours=3, minutes=33, seconds=15)),
        20,
        "Oversea".encode('utf-8'),
        "轉帳".encode('utf-8')
    )
    tx3 = Transaction(
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(minutes=-33, seconds=15)),
        20,
        "IBMB".encode('utf-8'),
        "轉帳".encode('utf-8')
    )
    txList = [tx1, tx2, tx3]*1000000

    print("Start Testing ..")
    START = time.time()
    for ruleId in range(len(ruleList)):
        rule = ruleList[ruleId]
        dateTimeStart = dateTimeStartList[ruleId]
        print("rule %s:" % (ruleId+1))
        print("Start date time of rule test: ", CDateTime2Py(dateTimeStart))
        print("Threshold of amount: %f" % rule.amtThresh)
        trigger = rule.Run(txList, dateTimeStart)
        if trigger:
            print("=== Rule %s had been triggered. ===" % (ruleId+1))
    print("done")
    END = time.time()
    print("Spent %f seconds" % (END - START))
