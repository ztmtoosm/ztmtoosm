var WizualizacjaDrzewa = function (from, time, linia, div, type)
{
	this.type=type;
	div.innerHTML="";
	this.from = from;
	this.time = time;
	this.linia = linia;
	this.div = div;
	if(type==1)
		this.tabela = loadJSON(kursSciezka(from, linia, time));
	else
		this.tabela = loadJSON(polaczeniaSciezka(from, 2, time));
	this.tabelaDIV = [];
	this.ladujDIV(0, this.tabela.length);
	if(type==0)
	{
		this.more1 = document.createElement("DIV");
		this.more1.implicit = this;
		this.more1.innerHTML="<a href=\"javascript:void(0)\">WCZEŚNIEJ...</a>";
		this.div.appendChild(this.more1);
		this.more1.onclick=function(e)
		{
			this.implicit.rozszerzUp();
		};
	}
	for(var i=0; i<this.tabelaDIV.length; i++)
	{
		this.div.appendChild(this.tabelaDIV[i]);
	}
	if(type==0)
	{
		this.more2 = document.createElement("DIV");
		this.more2.implicit = this;
		this.more2.innerHTML="<a href=\"javascript:void(0)\">PÓŹNIEJ...</a>";
		this.div.appendChild(this.more2);
		this.more2.onclick=function(e)
		{
			this.implicit.rozszerzDown();
		};
	}
}
WizualizacjaDrzewa.prototype.rozszerzUp = function ()
{
	var tabela2 = loadJSON(polaczeniaSciezka(this.from, 1, this.tabela[0].time));
	var s1 = tabela2.length;
	var tabela3 = [];
	for(var i=0; i<tabela2.length; i++)
	{
		tabela3[i]=null;
	}
	for(var i=0; i<this.tabela.length; i++)
	{
		tabela2[tabela2.length]=this.tabela[i];
		tabela3[tabela3.length]=this.tabelaDIV[i];
	}
	this.tabela=tabela2;
	this.tabelaDIV=tabela3;
	this.ladujDIV(0, s1);
	this.div.innerHTML="";
	this.div.appendChild(this.more1);
	for(var i=0; i<this.tabelaDIV.length; i++)
	{
		this.div.appendChild(this.tabelaDIV[i]);
	}
	this.div.appendChild(this.more2);
}
WizualizacjaDrzewa.prototype.rozszerzDown = function ()
{
	var tabela2 = loadJSON(polaczeniaSciezka(this.from, 0, this.tabela[this.tabela.length-1].time));
	for(var i=0; i<tabela2.length; i++)
	{
		this.tabela[this.tabela.length]=tabela2[i];
	}
	this.ladujDIV(this.tabela.length-tabela2.length, this.tabela.length);
	this.div.innerHTML="";
	this.div.appendChild(this.more1);
	for(var i=0; i<this.tabelaDIV.length; i++)
	{
		this.div.appendChild(this.tabelaDIV[i]);
	}
	this.div.appendChild(this.more2);
}
WizualizacjaDrzewa.prototype.ladujDIV = function (from, to)
{
	for(var i=from; i<to; i++)
	{
		var nowy_postoj = document.createElement("DIV");
		var nowy_postoj_linia = document.createElement("div");
		var nowy_postoj_godzina = document.createElement("div");
		var nowy_postoj_link = document.createElement("a");
		nowy_postoj_link.href="javascript:void(0)";
		nowy_postoj_link.title="pokaż kurs";
		nowy_postoj.className="nowy_postoj";
		if(i<this.center)
		{
			nowy_postoj.className="nowy_postoj malelitery";
		}
		nowy_postoj_linia.className="nowystopname";
		nowy_postoj_godzina.className="nowystoptime";
		nowy_postoj_link.className="nowy_postoj_link";
		nowy_postoj.appendChild(nowy_postoj_linia);
		nowy_postoj.appendChild(nowy_postoj_godzina);
		nowy_postoj_linia.appendChild(nowy_postoj_link);
		nowy_postoj.index=i;
		this.tabelaDIV[i]=nowy_postoj;
		nowy_postoj.implicit=this;
		nowy_postoj_link.implicit=nowy_postoj;
		var wpisz=nowy_postoj_link;
		if(this.type==0)
			wpisz.innerHTML=this.tabela[i].linia+" ("+this.tabela[i].idname+")";
		else
			wpisz.innerHTML=this.tabela[i].name;
		nowy_postoj_godzina.innerHTML=stringTime(this.tabela[i].time);
		nowy_postoj_link.onclick=function(e)
		{
			this.implicit.rozwin();
		}
		nowy_postoj.rozwin=function()
		{
			var id=this.implicit.from;
			var teraz=this.implicit.tabela[this.index];
			var extra = new WizualizacjaPodrozy(teraz.id, teraz.linia, teraz.time, 1);
		}
	}
}

