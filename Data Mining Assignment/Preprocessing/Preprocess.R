# library("discretization")
# library("arules")
library("infotheo")
setwd("C:/Users/Philip/Downloads/DM Project/student")
student<-read.table("student-mat.txt",row.names=NULL,header=TRUE, sep=";")
# y<-read.csv(file="student-mat.csv",sep=";")
# x<-(y[,c(1,2,4:29)])
# cp<-chi2(t(student[,30]),0.5,2)$cutp
# Dd<-chi2(t(student[,30]),0.5,0.05)$Disc.data


# discretization
student[,30]<-discretize(student[,30], disc="equalfreq", nbins=round(sqrt(length(levels(as.factor(student[,30]))))) )
student[,3]<-discretize(student[,3], disc="equalfreq", nbins=round(sqrt(length(levels(as.factor(student[,30]))))) )
x<-student[,c(1:6,11,14:17,19,21:23,25,29,30)]

#7,8,9,10,12,13,18,20,24,26,27,28
# binarization
Ds<-NULL
n<-NULL
for( i in 1:dim(x)[2])
{
  cc<-levels(as.factor(x[,i]))
  z<-matrix(0,dim(x)[1],length(cc))
  colnames(z)<-cc
  for(j in cc)
  {
    z[,j]<-1*(x[,i]==j)
    n<-c(n,paste(colnames(x)[i],j,sep="="))
    
  }
  Ds<-cbind(Ds,z)
}
colnames(Ds)<-n
write.table(Ds,file="C:/Users/Philip/Downloads/DM Project/student/Binarized.dat")

# converting to transactions
zz<-matrix(0,dim(x)[1],dim(x)[2]+1)
for(i in 1:dim(x)[1])
{
  xc<-colnames(Ds)[which(Ds[i,]==1)]
  zz[i,]<-c(length(xc),xc)
  
}
write.table(zz,file="C:/Users/Philip/Downloads/DM Project/student/Final.dat",sep="\t",col.names=FALSE,quote=FALSE)


####################################################################################################################
# rule mining
rules<-apriori(Ds,parameter = list(minlen=2, maxlen=3, supp=0.1, conf=0.8))
inspect(rules)

# pruning
rules.sorted <- sort(rules, by="lift")
subset.matrix <- is.subset(rules.sorted, rules.sorted)
subset.matrix[lower.tri(subset.matrix, diag=T)] <- NA
redundant <- colSums(subset.matrix, na.rm=T) >= 1
which(redundant)
# remove redundant rules
rules.pruned <- rules.sorted[!redundant]
inspect(sort(rules.pruned,by="support", decreasing = FALSE))



