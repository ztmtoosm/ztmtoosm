 jQuery.getFeed({
	    url: 'rss.xml',
	       success: function(feed) {
		       for(var i = 0; i < feed.items.length; i++) {
		       var item = feed.items[i];

var html = '<a href="'+item.link+'">'+item.description+'</a><br>';
/*+ '<a href="'
+ item.link
+ '">'
+ item.title
+ '</a>'
+ '</h3>';

html += '<div class="updated">'
+ item.updated
+ '</div>';

html += '<div>'
+ item.description
+ '</div>';*/

	  var arrayOfStrings = item.title.split(/(, |: )/);
console.log(arrayOfStrings);
for(var j=1; j<arrayOfStrings.length; j++)
{
	var abc = document.getElementById("pelne"+arrayOfStrings[j]);
	if(abc!=null)
{
	var def = abc.getElementsByClassName("list-group");
	var ghi = document.createElement("li");
	ghi.innerHTML=html;
	ghi.setAttribute("class", "list-group-item list-group-item-info");
	jQuery(def).append(ghi);
}
}
		       }
		       }});
