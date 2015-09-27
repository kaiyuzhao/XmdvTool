#include "util/algo/mds/MDS_Kruskal.h"
#include "util/algo/mds/nrutil.h"

Kruskal_node::Kruskal_node():
	i (0),
	j (0),
	dissim (0),
	disparity (0)
{}

int	Kruskal_node::operator > (const Kruskal_node &kn)
{
	if (dissim > kn.dissim ) return 1;
	if (dissim == kn.dissim) {
		if (disparity> kn.disparity ) return 1;
		else return 0;
	}
	else return 0;
}


int	Kruskal_node::operator < (const Kruskal_node &kn)
{
	return (*this>kn? 0:1);
}

Kruskal_node& Kruskal_node::operator = (const Kruskal_node &kn)
{
	i=kn.i;
	j=kn.j;
	dissim=kn.dissim ;
	disparity=kn.disparity ;
	return *this;
}

std::istream& operator>> (std::istream& is, Kruskal_node&)
{
	return is;
}

std::ostream& operator<< (std::ostream& os, Kruskal_node& kn)
{
	char tbl='\t';
	return (os<<kn.i<<tbl<<kn.j<<tbl<<kn.dissim <<tbl<<kn.disparity <<std::endl);
}


void Kruskal_Array::sort_disparity ()
{

	int flag=1;
	Kruskal_node* t=v;

	int debug=0;

	while (flag) {
		v=t;
		flag=0;
		for (int i=0; i<len-1; i++) {
			if (v->disparity>(v+1)->disparity) {
				flag=1;
				//double t=v->disparity ;
				//v->disparity =(v+1)->disparity ;
				//(v+1)->disparity =t;

				double avg=(v->disparity+(v+1)->disparity)/2.0;
				v->disparity =(v+1)->disparity=avg;
			}
			v++;
		}
		//cout<<"debug=:"<<debug<<endl;
		debug++;
		if (debug>len) break;
	}
	v=t;

}

MDS_Kruskal::MDS_Kruskal():
	MDS_Embedding(),
	Dij(),
	Xik(),
	Disparity(),
	stress (0.0),
	krusk ()
{}


void MDS_Kruskal::init ()
{
	Xik= *this;

	//cout<<"Xik:"<<endl<<Xik<<endl;


	//initialize the distantance matrix
	Dij=Xik.row_p2sqr_dist();

	krusk.setSize(len0*(len0-1)/2);

	standardize();
	//Disparity=Dij;

}



void MDS_Kruskal::standardize ()
{
	//cout<<"Dij:"<<endl<<Dij<<endl;

	double c=Dij.p2sqr();
	Dij *= 1/sqrt(c);
	Xik *= 1/sqrt(c);

	//cout<<"Xik:"<<endl<<Xik<<endl;
	//cout<<"Dij:"<<endl<<Dij<<endl;



}


void MDS_Kruskal::initKruskList ()
{
	int index=0;
	for (int i=0; i<Dissim.length0(); i++) {
		for (int j=0; j<i; j++) {
			Kruskal_node kn;
			kn.i=i;
			kn.j=j;
			kn.dissim =Dissim(i,j);
			kn.disparity =Disparity(i,j);
			krusk[index]=kn;
			index++;
		}
	}
}

/*
 * see text Multidimensional scaling by Mark L. Davison p108
 */
#include <iomanip>
void MDS_Kruskal::nonmetric ()
{
	krusk.bubble_sort();			//ascending order
	krusk.sort_disparity();
}


void MDS_Kruskal::fromKruskList ()
{
	for (int k=0; k<krusk.len; k++)
			Disparity(krusk[k].i, krusk[k].j)=krusk[k].disparity;
	Disparity.mirror();
	Disparity.diagonal().set2(0.0);
}

//see text page 112 for detail algorithms
void MDS_Kruskal::metric ()
{
	MDS_Matrix X(len0, len1);	//temperal variant for Xik
	MDS_Matrix Div=Disparity/Dij;
	for (int i=0; i<len0; i++) {
		for (int k=0; k<len1; k++) {
			MDS_Vector row_i=1.0-Div.row(i);
			MDS_Vector col_k=Xik(i,k)-Xik.col(k);
			X(i,k)=Xik(i,k)-row_i*col_k/len0;
		}
	}
	Xik=X;
	Dij=Xik.row_p2sqr_dist();
}


double MDS_Kruskal::s1 ()
{
	MDS_Matrix Diff=Disparity-Dij;
	return sqrt(Diff.p2sqr()/Dij.p2sqr());
}


double MDS_Kruskal::s2 ()
{
	MDS_Matrix Diff=Disparity-Dij;
	MDS_Matrix D=Dij-Dij.mean();
	return sqrt(Diff.p2sqr()/D.p2sqr());
}


double MDS_Kruskal::ss1 ()
{
	MDS_Matrix Disparity_sqr(Disparity.len0,Disparity.len1),Dij_sqr(Dij.len0,Dij.len1);
	Disparity.pointMulAssign(Disparity,Disparity_sqr);
	Dij.pointMulAssign(Dij,Dij_sqr);

	MDS_Matrix Diff=Disparity_sqr-Dij_sqr;
	return sqrt(Diff.p2sqr()/Dij_sqr.p2sqr());
}


double MDS_Kruskal::ss2 ()
{
	MDS_Matrix Disparity_sqr(Disparity.len0,Disparity.len1),Dij_sqr(Dij.len0,Dij.len1);
	Disparity.pointMulAssign(Disparity,Disparity_sqr);
	Dij.pointMulAssign(Dij,Dij_sqr);
	MDS_Matrix Diff=Disparity_sqr-Dij_sqr;
	MDS_Matrix D=Dij_sqr-Dij_sqr.mean();
	return sqrt(Diff.p2sqr()/D.p2sqr());
}

void MDS_Kruskal::iterateOnce ()
{
	//set initial disparity data to estimated distance
	Disparity=Dij;

	initKruskList();

	//cout<<"before nnonmetric"<<endl<<krusk<<endl;
	nonmetric();
	//cout<<"after nonmetric"<<endl<<krusk<<endl;

	fromKruskList();
	metric();

	standardize();
}


void MDS_Kruskal::kruskal_scaling (double str)
{
	init();
	while (stress>str) iterateOnce ();
}


void MDS_Kruskal::printStress()
{
	std::cout<<"stresses:"<<std::endl;
	std::cout<<"s1="<<s1()<<std::endl;
	std::cout<<"s2="<<s2()<<std::endl;
	std::cout<<"ss1="<<ss1()<<std::endl;
	std::cout<<"ss2="<<ss2()<<std::endl;
}
