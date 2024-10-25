/*此处的通道名字要和程序中的是一致的，否则是无法发送信息的*/
var external0 = null;              // 方式发送信息方，其实也就是当前的客户端
var external_10002 = null;         // 一个客户端可以接收多个发送信息，所以为了区分是谁发送来的数据我们使用external_用户的id来表示
var external_10003 = null;
var external_10004 = null;
var external_10005 = null;
var external_10006 = null;
var external_10007 = null;
var external_10009 = null;
var external_10010 = null;
var external_10011 = null;
var external_10012 = null;
var external_10013 = null;



String.prototype.format = function() {  
	if(arguments.length == 0) return this;  
	var obj = arguments[0];  
	var s = this;  
	for(var key in obj) {  
		s = s.replace(new RegExp("\\{\\{" + key + "\\}\\}", "g"), obj[key]);  
	}  
	return s;  
}  

new QWebChannel(qt.webChannelTransport,
	function(channel) {
		external0 = channel.objects.external0;
		external_10002 = channel.objects.external_10002;
external_10003 = channel.objects.external_10003;
external_10004 = channel.objects.external_10004;
external_10005 = channel.objects.external_10005;
external_10006 = channel.objects.external_10006;
external_10007 = channel.objects.external_10007;
external_10009 = channel.objects.external_10009;
external_10010 = channel.objects.external_10010;
external_10011 = channel.objects.external_10011;
external_10012 = channel.objects.external_10012;
external_10013 = channel.objects.external_10013;

	}
);

/*appendHtml之所以只有一个，是因为其表示发送信息的函数，一个qq登录其实只会存在一个发送者，但是recv对应的
接收函数去很多，是因为一个用户的接收方会很多*/
function appendHtml0(msg){
	$("#placeholder").append(external0.msgRHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};

function recvHtml_10002(msg){
	$("#placeholder").append(external_10002.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10003(msg){
	$("#placeholder").append(external_10003.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10004(msg){
	$("#placeholder").append(external_10004.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10005(msg){
	$("#placeholder").append(external_10005.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10006(msg){
	$("#placeholder").append(external_10006.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10007(msg){
	$("#placeholder").append(external_10007.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10009(msg){
	$("#placeholder").append(external_10009.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10010(msg){
	$("#placeholder").append(external_10010.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10011(msg){
	$("#placeholder").append(external_10011.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10012(msg){
	$("#placeholder").append(external_10012.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
function recvHtml_10013(msg){
	$("#placeholder").append(external_10013.msgLHtmlTmpl.format(msg));
	window.scrollTo(0,document.body.scrollHeight); ;  
};
