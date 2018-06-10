import numpy as np
import pandas as pd
import math
import sys


def PCC(ratings, kind='user'):
    print("PCC")
    if kind == 'user':
        axmax = 0
        axmin = 1
    sim = np.zeros((ratings.shape[axmax], ratings.shape[axmax]))
    for u in range(ratings.shape[axmax]):
        print("user :" + str(u))
        for uprime in range(u+1, ratings.shape[axmax]):
            u_mean = np.mean(ratings[u])
            uprime_mean = np.mean(ratings[uprime])

            rui_sqrd = 0.
            ruprimei_sqrd = 0.
            for i in range(ratings.shape[axmin]):
                sim[u, uprime] += (ratings[u, i] - u_mean) * (ratings[uprime, i] - uprime_mean)
                rui_sqrd += (ratings[u, i] - u_mean) * (ratings[u, i] - u_mean)
                ruprimei_sqrd += (ratings[uprime, i] - uprime_mean) * (ratings[uprime, i] - uprime_mean)
            sim[u, uprime] /= math.sqrt(rui_sqrd * ruprimei_sqrd)

    sim = sim +sim.T
    for u in range(ratings.shape[axmax]):
        sim[u,u] = 1

    return sim

fileName = sys.argv[1]

names = ['user_id', 'item_id', 'rating', 'timestamp']
df = pd.read_csv(fileName, sep='\t', names=names)

n_users = df.user_id.max()
n_items = df.item_id.max()
print(str(n_users) + " users")
print(str(n_items) + " items")

ratings = np.zeros((n_users, n_items))
test = np.zeros((n_users, n_items))
for row in df.itertuples():
    ratings[row[1]-1, row[2]-1] = row[3]


user_similarity = PCC(ratings, kind='user')
for u in range(np.size(user_similarity, 0)) :
    for u_prime in range(np.size(user_similarity, 1)) :
        if user_similarity[u, u_prime] < 0.2 :
            user_similarity[u, u_prime] = .0


print("Aggergation")
for u in range(n_users) :
    print("user: " + str(u))
    for C in range(np.size(user_similarity, 0)):
        if(user_similarity[u, C] == 0) :
            continue
        for u_prime in range(n_items) :
            test[u, u_prime] +=  user_similarity[u,C] * ratings[C, u_prime]


for u in range(n_users) :
    for u_prime in range(n_items):
        if(ratings[u, u_prime] != 0) :
            test[u, u_prime] = 0


for u in range(n_users) :
    for u_prime in range(n_items):
        if(test[u, u_prime] >5) :
            test[u, u_prime] = 5
        elif(test[u, u_prime] <1):
            test[u, u_prime] = 0


outputName = fileName.split('.')[0] + ".base_prediction.txt"
try:
    fp = open(outputName, 'w')
except:
    print("파일을 쓸 수 없습니다. 다시 시도해주세요.\n")

for u in range(n_users) :
    for u_prime in range(n_items):
        if(test[u, u_prime] != 0) :
            fp.write("%d\t%d\t%d\n" % (u+1, u_prime+1, np.round(test[u, u_prime])))
