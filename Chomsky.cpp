#include <iostream>
#include <set>
#include <string>
#include <map>
#include <vector>
using namespace std;

map <string, set<string>> P;	//Productii
set <string> N;					//Neterminale
set <char> Sigma;				//Terminale
char caracter;

void configGrammar() {
	

    P["S"].insert("aB");
    P["S"].insert("AC");
    P["A"].insert("a");
    P["A"].insert("ASC");
    P["A"].insert("BC");
    P["A"].insert("aD");
    P["B"].insert("b");
    P["B"].insert("bS");
    P["C"].insert("@");
    P["C"].insert("BA");
    P["D"].insert("abC");
    P["E"].insert("aB");

	N.insert("S");
    N.insert("A");
    N.insert("B");
    N.insert("C");
    N.insert("D");
    N.insert("E");
	Sigma.insert('a'); Sigma.insert('b');


	caracter = 'F';

	
}

void uselessProductions() {
	set <string> n1;
	for (auto n : N)
		for (auto p : P[n]) {
			int i = 0;
			while (i < p.size() && N.find(string(1, p[i])) == N.end())
				i++;
			if (i == p.size()) {
				n1.insert(n);
				break;
			}
		}
	int ok;
	do {
		ok = 0;
		for (auto n : N)
			if (n1.find(n) == n1.end()) {
				for (auto p : P[n]) {
					int i = 0;
					while (i < p.size() && (N.find(string(1, p[i])) == N.end() || n1.find(string(1, p[i])) != n1.end()))
						i++;
					if (i == p.size()) {
						n1.insert(n);
						ok = 1;
						break;
					}
				}
			}
	} while (ok);

	//Simboluri din N care sunt accesibile
	vector <string> N2;
	set <string> n2;
	n2.insert("S");
	N2.push_back("S");
	int i = 0;
	while (i < N2.size()) {
		string n = N2[i];
		for (auto p : P[n]) {
			for (int j = 0; j < p.size(); j++)
				if (N.find(string(1, p[j])) != N.end() && n2.find(string(1, p[j])) == n2.end()) {
					n2.insert(string(1, p[j]));
					N2.push_back(string(1, p[j]));
				}
		}
		i++;
	}

	//Elementele comune = ce trebuie sa ramana (in n1)
	set<string>::iterator it = n1.begin();
	while (it != n1.end()) {
		if (n2.find(*it) == n2.end()) {
			N.erase(*it);
			P.erase(*it);
			it=n1.erase(it);

		}
		else{
			n2.erase(*it);
			it++;
		}
	}
	for (auto s : n2) {
		N.erase(s);
		P.erase(s);
	}
		
	//Stergem productiile care contin neterminale care nu sunt in N
	for (auto n : N) {
		set<string> Pnou;
		for (auto p : P[n]) {
			int i = 0;
			while (i < p.size()) {
				if (p[i] != '@' && Sigma.find(p[i]) == Sigma.end() && N.find(string(1, p[i])) == N.end())
					break;
				i++;
			}
			if (i == p.size())
				Pnou.insert(p);
		}
		P[n] = Pnou;
	}
}

void lambdaProductions() {
	set <string> eliminare;
	int ok = 0;
	//Pentru fiecare neterminal
	for (auto n : N) {
		if (P[n].find("@") != P[n].end()) {
			ok = 1;
			if (P[n].size() == 1) {
				eliminare.insert(n);
				for (auto nn : N)
					if (nn != n) {
						set<string> Pnou;
						for (auto p : P[nn]) {
							if (p.size() == 1 && p == n) {
								p = "@";
							}
							if (p.size() > 1 && p.find(n) != string::npos) {
								int poz = p.find(n);
								p.erase(poz, 1);
							}

							Pnou.insert(p);
						}
						P[nn] = Pnou;
					}
			}
			else {
				P[n].erase("@");
				for (auto nn : N)
					{
						set<string> Pnou;
						string vechip = "";
						for (auto p : P[nn]) {
							if (p.size() > 1 && p.find(n) != string::npos) {
								vechip = p;
								int poz = p.find(n);
								p.erase(poz, 1);
							}
							Pnou.insert(p);
							if (vechip != "") {
								Pnou.insert(vechip);
								vechip = "";
							}
						}
						P[nn] = Pnou;
					}
			}
		}
	}
	for (auto s : eliminare) {
		N.erase(s);
		P.erase(s);
	}
	if (ok == 1)
		lambdaProductions();
}

void eliminateStart() {
	N.insert("S0");
	P["S0"].insert("S");
}

void unitProductions() {
	int ok;
	do {
		ok = 0;
		for (auto n : N) {
			set<string> Pnou;
			for (auto p : P[n]) {
				if (p != n && N.find(p) != N.end()) {
					ok = 1;
					for (auto pp : P[p])
						Pnou.insert(pp);
				}
				else
					Pnou.insert(p);
			}
			P[n] = Pnou;
		}
	} while (ok);
}

void eliminateTerminals() {
	//Productiile noi adaugate
	set <pair<char, char>> newAdd;

	for (auto n : N) {
		set<string> Pnou;	//Retin toate productiile (cu modificari, daca e cazul)
		for (auto p : P[n]) {
			if (p.size() >= 2) {
				for (int i = 0; i < p.size(); i++) {
					if (Sigma.find(p[i]) != Sigma.end()) {
						int ok = 0;
						char caracternou;
						for (auto s : newAdd)
							if (s.second == p[i]) {
								caracternou = s.first;
								ok = 1;
								break;
							}
						if (ok == 1)
							p[i] = caracternou;
						else {
							caracter++;
							if (caracter == 'S')
								caracter++;
							string sc = string(1, caracter);
							P[sc].insert(string(1, p[i]));
							newAdd.insert({ caracter,p[i] });
							p[i] = caracter;
						}
					}
				}
			}
			Pnou.insert(p);
		}
		P[n] = Pnou;
	}

	//Adaug neterminalele noi
	if (newAdd.size() != 0) {
		for (auto s : newAdd)
			N.insert(string(1, s.first));
	}
}

void eliminateMoreNonTerminals() {
	//Neterminalele noi adaugate
	set <pair<char,string>>newAdd;
	int ok=0;
	do {
		ok = 0;
		for (auto n : N) {
			set<string> Pnou;	//Retin toate productiile (cu modificari, daca e cazul)
			for (auto p : P[n]) {
				if (p.size() >= 3) {
					ok = 1;
					string rest = p.substr(1);

					int exista = 0;
					char caracternou;
					for (auto s : newAdd)
						if (s.second == rest) {
							exista = 1;
							caracternou = s.first;
							break;
						}
					if (exista == 1)
						p = p[0] + string(1, caracternou);
					else {
						//Neterminalul de inserat
						caracter++;
						if (caracter == 'S')
							caracter++;
						string sc = string(1, caracter);
						p = p[0] + sc;
						P[sc].insert(rest);
						newAdd.insert({ caracter,rest });
					}
				}
				Pnou.insert(p);
			}
			P[n] = Pnou;
		}
		//Adaug neterminalele noi
		if (newAdd.size() != 0) {
			for (auto s : newAdd)
				N.insert(string(1, s.first));
		}
	} while (ok == 1);
}


int main() {
	configGrammar();
	uselessProductions();
	lambdaProductions();
	unitProductions();
	uselessProductions();
	eliminateTerminals();
  eliminateMoreNonTerminals();

	for (auto p : P) {
		cout << p.first << "->";
		for (auto e : p.second)
			cout << e << ",";
		cout << endl;
	}

	return 0;
}
