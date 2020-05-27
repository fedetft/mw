/***************************************************************************
 *   Copyright (C) 2018 by Terraneo Federico                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include <map>
#include <string>
#include <stdexcept>
#include <cctype>
#include <cassert>
#include <iostream>

using namespace std;

class MwComputation
{
public:
	MwComputation(const string& formula);
	void print() const;
	float getMw() const { if(result>0.f) return result; else throw index; }

private:
	/*
	 * S -> M | MS | "(" S ")" [ N ] [ S ]
	 * M -> E [ N ]
	 * N -> {09}+
	 * E -> "H" | "He" | ...
	 */
	float s();
	float m();
	float n();
	float e();

	char peek()
	{
		if(index>=f.length()) return '\0';
		return f.at(index);
	}

	const std::string f;
	size_t index;
	float result=-1.0f;
	static const map<string,float> mw;
};

MwComputation::MwComputation(const string& formula) : f(formula), index(0)
{
	try {
		result=s();
	} catch(size_t& index) {
		this->index=index;
	}
}

void MwComputation::print() const
{
	if(result>0.f)
	{
		cout<<result<<endl;
	} else {
		cerr<<"Failed to parse "<<f<<endl;
		cerr<<"                ";
		for(size_t i=0;i<index;i++) cerr<<' ';
		cerr<<"^ from here"<<endl;
	}
}

float MwComputation::s()
{
	if(isalpha(peek()))
	{
		float result=m();
		char next=peek();
		if(next==')' || next=='\0') return result; // S -> M
		else return result+s(); // S -> MS
	}
	if(peek()=='(')
	{
		index++;
		float result=s();
		if(peek()!=')') throw index;
		index++;
		if(isdigit(peek())) result*=n();
		char next=peek();
		if(next==')' || next=='\0') return result; // S -> "(" S ")" [ N ]
		else return result+s(); // S -> "(" S ")" [ N ] S
	}
	throw index;
}

float MwComputation::m()
{
	float result=e();
	if(isdigit(peek())) return result*n(); // M -> E N
	else return result; // M -> E
}

float MwComputation::n()
{
	assert(index<f.length());
	size_t advance=0;
	int result=stoi(f.substr(index),&advance);
	index+=advance;
	return static_cast<float>(result);
}

float MwComputation::e()
{
	assert(index<f.length());
	if(f.length()-index>=2)
	{
		auto result=mw.find(f.substr(index,2));
		if(result!=mw.end())
		{
			index+=2;
			return result->second;
		}
	}
	auto result=mw.find(f.substr(index,1));
	if(result!=mw.end())
	{
		index+=1;
		return result->second;
	}
	throw index;
}

const map<string,float> MwComputation::mw={
{"H", 1.007},{"He",4.003},
{"Li",6.941},{"Be",9.012},{"B", 10.81},{"C", 12.01},{"N", 14.01},{"O", 16.00},{"F", 19.00},{"Ne",20.18},
{"Na",22.99},{"Mg",24.31},{"Al",26.98},{"Si",28.09},{"P", 30.98},{"S", 32.07},{"Cl",35.45},{"Ar",39.95},
{"K", 39.10},{"Ca",40.08},{"Sc",44.96},{"Ti",47.87},{"V", 50.94},{"Cr",52.00},{"Mn",54.94},{"Fe",55.85},{"Co",58.93},{"Ni",58.69},{"Cu",63.54},{"Zn",65.38},{"Ga",69.72},{"Ge",72.63},{"As",74.92},{"Se",78.96},{"Br",79.90},{"Kr",83.80},
{"Rb",85.47},{"Sr",87.62},{"Y", 88.90},{"Zr",91.22},{"Nb",92.91},{"Mo",95.96},{"Tc",97.91},{"Ru",101.1},{"Rh",102.9},{"Pd",106.4},{"Ag",107.9},{"Cd",112.4},{"In",114.8},{"Sn",118.7},{"Sb",121.8},{"Te",127.6},{"I", 126.9},{"Xe",131.3},
{"Cs",132.9},{"Ba",137.3},{"La",138.9},{"Ce",140.1},{"Pr",140.9},{"Nd",144.2},{"Pm",145.0},{"Sm",150.4},{"Eu",151.9},{"Gd",157.3},{"Tb",158.9},{"Dy",162.5},{"Ho",164.9},{"Er",167.3},{"Tm",168.9},{"Yb",173.1},{"Lu",175.0},{"Hf",178.5},{"Ta",181.0},{"W", 183.8},{"Re",186.2},{"Os",190.2},{"Ir",192.2},{"Pt",195.1},{"Au",197.0},{"Hg",200.6},{"Tl",204.4},{"Pb",207.2},{"Bi",209.0},{"Po",209.0},{"At",210.0},{"Rn",222.0},
{"Fr",223.0},{"Ra",226.0},{"Ac",227.0},{"Th",232.0},{"Pa",231.0},{"U", 238.0},{"Np",237.0},{"Pu",244.0},{"Am",243.0},{"Cm",247.0},{"Bk",247.0},{"Cf",251.0},{"Es",252.0},{"Fm",257.0},{"Md",258.0},{"No",259.0},{"Lr",263.0},{"Rf",261.0},{"Db",262.0},{"Sg",266.0},{"Bh",264.0},{"Hs",277.0},{"Mt",268.0},{"Ds",271.0},{"Rg",272.0},{"Cn",285.0}
};

int main(int argc, char *argv[])
{
	if(argc==2) MwComputation(argv[1]).print();
	else {
		string line;
		getline(cin,line);
		MwComputation(line).print();
	}
	return 0;
}
