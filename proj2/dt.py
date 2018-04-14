import numpy as np
import math
import sys

def Info (count_list):
    ret = 0
    for i in count_list :
        if i == 0 :
            return 0
        ret -= i/sum(count_list) *math.log(i/sum(count_list),2)
    return ret

def Info_A (list):
    ret = 0
    rows = np.unique(list[0])
    cols = np.unique(list[1])

    Tr_list = list.T
    x_list = [[0 for x in range(cols.size)] for y in range(rows.size)]
    for i in Tr_list :
        arr1 = np.where(i[0]== rows)
        arr2 = np.where(i[1]== cols)
        if(x_list[arr1[0][0] ][arr2[0][0] ] == 0) :
            x_list[arr1[0][0]][arr2[0][0]] = 1
        else :
            x_list[arr1[0][0]][arr2[0][0]] += 1
 #       try : x_list[1][j] += 1
 #       except : x_list[1][j] =1
    for i in x_list :
        ret += (sum(i)/list[0].size * Info(i))
    return ret

def func (list, idx_list, node) :
    # 모든 어트리뷰트를 다 탐색했을 때 재귀 종료
    if list.shape[1] - 1 == len(idx_list) :
        node.append([list.T[list.shape[1] - 1][0]])
        return
    # 완벽히 갈렸을 때
    if np.unique(list.T[list.shape[1] - 1]).size == 1 :
        node.append([list.T[list.shape[1] - 1][0]])
        return
    idx = -1
    a = list[:, 0:-1]
    c = list[:, -1]
    min = 1
    count = 0

    for i in a.T:
    #    print(i, "a")
        if count in idx_list:
            count += 1
            continue

        b = np.array([i, c])
   #     print(b, "b")
        if min > Info_A(np.array(b)):
            min = Info_A(np.array(b))
            idx = count
        count += 1
    # 모두 만족한 경우기 때문에 더이상 진행하지않는다
    idx_list += [idx]


    # 튜플 개수
    # print (list.shape[0] - 1)
    # 어트리뷰트 개수 (클래스 제외)
    # print (list.shape[1] - 1)
    attr = np.unique(list[:, idx])
    # xx_list = np.empty([attr.size,], dtype=object)
    xx_list = [[] for y in range(attr.size)]

    for i in list:
        arr1 = np.where(i[idx] == attr)
        #    np.append(xx_list[arr1[0][0]], i)
        xx_list[arr1[0][0]].append(i)
    for i in xx_list:
#        node.append(i[0][idx].tolist)

        node.append([idx, i[0][idx]])

   #     print([i[0][idx]])
   #     print(idx_list)
   #     print(np.array(i))
   #     print(node)
        func(np.array(i), idx_list[:], node[len(node) - 1])

def testfunc (test,tree) :
    count = 0
   # print(tree)
    if len(tree) == 1:
        return tree[0][0]

    while count != len(tree):
        # print(test[tree[count][0]])

        if test[tree[count][0]] == tree[count][1]:
            return testfunc(test, tree[count][2:])
        count += 1
    count = 0
    
    # 동일한 조건을 못찾았으면 비슷한걸로 가게한다
    while count != len(tree):
        # print(test[tree[count][0]])
        if test[tree[count][0]] < tree[count][1]:
            return testfunc(test, tree[count][2:])
        count += 1

    return testfunc(test, tree[-1][2:])


def supertest (test_list, tree):
    ret_list = np.insert(test_list, len(test_list[0]), values=0, axis=1)
    count =0
    for i in test_list:
        ret_list[count][-1] = testfunc(test_list[count], tree)
        count += 1
    return ret_list
#list = np.genfromtxt('dt_train1.txt',dtype='str')
#test_list = np.genfromtxt('dt_test1.txt',dtype='str')
list = np.genfromtxt(sys.argv[1],dtype='str')
test_list = np.genfromtxt(sys.argv[2],dtype='str')

idx_list =[]
tree = []
func(list[1:,:], idx_list, tree)
#print("trained tree")
#print(tree)
#print(test_list)

output = supertest(test_list[1:,:],tree)
output = np.vstack([list[0], output])
#print("output")
#print(output)
#for i in test_list :
np.savetxt(sys.argv[3], output, fmt='%s', delimiter='\t')


