import numpy as np
import math
import random
import sys

numOfClassified = 0
def SortDB (fp) :
    #x축에 대해서 정렬, 그다음
    #y축에 대해서 정렬된 데이터를 반환
    #새로운 속성 : Classified 추가
    list = []
    while True:
        line = fp.readline()
        if not line:  # 파일의 내용을 한줄씩 읽고 다음줄이 없으면 while문 break
            break
        templist = line.split()
        templist[0] = int(templist[0])
        templist[1] = float(templist[1])
        templist[2] = float(templist[2])
        templist.append(None)
        list.append(templist)
    list.sort(key = lambda x: (x[1], x[2]))
    return list

def IsCorePoint (DB, p_idx, eps, minPts, cluster_idx) :
    global numOfClassified
    neighbors = []
    count = 0
 #   print(p_idx, numOfClassified)
    target_idx = np.where(np.array(DB)[:, 0] == p_idx)[0][0]
    target = DB[target_idx]
    if target[3] != None :
        return ["already done"]

    # DBSCAN에서 랜덤하게 뽑을 떄는 뺸다
    if cluster_idx != -1 :
        target[3] = cluster_idx
        numOfClassified += 1

    # p의 index에서 점점 아래로 탐색 , 범위는 x - eps<=x
    #  범위에 해당되는 놈들을 리스트로 저장
    #  범위에 해당되는 놈들의 개수를 count
    #자기 자신은 담지 않는다

    idx = target_idx
    while DB[target_idx][1] - eps <= DB[idx][1] :
        idx -= 1
        if idx < 0:
            break
        if math.sqrt(math.pow(DB[idx][1] - DB[target_idx][1], 2) + math.pow(DB[idx][2] - DB[target_idx][2], 2)) <= eps:
            count+=1
            if DB[idx][3] == None :
                neighbors.append(DB[idx][0])


    #  p의 index에서 점점 위로 올라가서 탐색, 범위는 x <= x +eps
    #  범위에 해당되는 놈들을 리스트로 저장
    #  범위에 해당되는 놈들의 개수를 count
    idx = target_idx
    while DB[idx][1] <= DB[target_idx][1] + eps:
        idx += 1
        if idx == len(DB):
            break
        if math.sqrt(math.pow(DB[idx][1] - DB[target_idx][1], 2) + math.pow(DB[idx][2] - DB[target_idx][2],2)) <= eps:
            count += 1
            if DB[idx][3] == None :
                neighbors.append(DB[idx][0])

    #  if minPts-1 <= count :
    #  core (리스트 반환)
    #  elif count == 0 :
    #  outlier
    #  else :
    #  border (리스트 반환)

    if minPts - 1 <= count :
        if cluster_idx == -1 :
            neighbors.append(DB[target_idx][0]) # 이부분은 본인을 포함시킨다
        return ["core", neighbors]
    elif count == 0:
        numOfClassified += 1
        target[3] = "outlier"
        return ["outlier"]
    else :
        return ["border", neighbors]

#    IsCorePoint의 반환값이 core리스트일경우 호출
def ExpandCluster(DB, cluster, eps, minPts, cluster_idx) :

   # print(str(cluster_idx + 1) + "번째 클러스터 생성중...\n")
    new_cluster = []
    new_cluster += cluster

    while len(cluster) != 0:
        #    cluster에 담은 point들이 core인지 border인지 IsCorePoint로 체크
        tempList = IsCorePoint(DB, cluster.pop(), eps, minPts, cluster_idx)
  #      print(cluster)
        #    core면 core리스트를 추가해줘야 하는데 original index를 통해 new_cluster와 중복체크를 함
        if tempList[0] == "core" :
            for item in tempList[1] :
                if item not in new_cluster:
                    new_cluster.append(item)
                    cluster.append(item)
        #    stack이 비면 종료
    return new_cluster


def DBSCAN (DB, numOfCluster, eps, minPts) :
    global numOfClassified
#    총 클러스터 갯수
    m = 0
#   SortDB (DB)
    cluster = []
    while numOfClassified != len(DB) and m < numOfCluster :
 #       print("==========================================================================")
 #       print(m)
 #      print("==========================================================================")

        #    p_idx = 랜덤하게 DB안의 요소를 선택
        p_idx = random.randint(1, len(DB) -1)
        tempList = IsCorePoint(DB, p_idx, eps, minPts, -1)
#    if(cluster의 개수 >=minPts-1) //core라면 expandCluster 를 호출
        if tempList[0] == "core"  :
            cluster.append([])
            #    stack에 cluster 리스트를 담는다
            cluster[m] =ExpandCluster(DB, tempList[1], eps, minPts, m)
        else :
            continue
        m += 1

    return cluster

#list = np.genfromtxt('input1.txt',dtype=[('index', int), ('x_value', float), ('y_value', float)])
#​list = np.genfromtxt(sys.argv[1],dtype='str')
fileName = sys.argv[1]
try:
    fp = open(fileName, 'r')
except:
    print("파일을 열 수 없습니다. 다시 시도해주세요.\n")

list = SortDB(fp)
#list = np.genfromtxt(sys.argv[1],dtype='str')
#test_list = np.genfromtxt(sys.argv[2],dtype='str')
cluster = DBSCAN(list,int(sys.argv[2]), int(sys.argv[3]) ,int(sys.argv[4]))
for i in range(0, len(cluster)):
    cluster[i].sort()

#print(cluster)


for i in range(0, len(cluster)):
    output = fileName.split('.')[0] + "_cluster_" + str(i) + ".txt"
    try:
        fp = open(output, 'w')
    except:
        print("파일을 쓸 수 없습니다. 다시 시도해주세요.\n")

    for item in cluster[i] :
        fp.write("%d\n" % item)
