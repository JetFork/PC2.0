var index = 0;
var runAfterInstalled=1;

var setup = {
    init: function () {
        this.initPage();//初始化安装界面
        this.initInstallPath();//初始化安装路径
        this.bindEvent();//绑定事件
    },
    initPage: function () {
        external.MainWnd.setSize(520, 420);
    	//external.MainWnd.setTitle("蜜蜂");
    	external.MainWnd.setWinTitleRect(0,0,480,50);
    	var width = (screen.width-520)/2;
    	var height = (screen.height-420)/2
    	external.MainWnd.setPos(width, height);

    	setTimeout(function(){
    		var ret = external.MainWnd.CheckInstall();
    		if(ret != 0)
    		{
    			var isNew = external.MainWnd.CheckIsNewVersion();
    			if( isNew == true)
    			{
                    handleDialog('cover', '您已安装该产品，是否覆盖安装？');
    			}
    			else
    			{
    				handleDialog('installed', '您已安装更高版本，不能覆盖安装！');
    			}
    		}
    		else
    		{
    			external.MainWnd.setSize(520, 420);
    			change_page();
    		}

        },1000);
        external.MainWnd.show();
    },
    initInstallPath: function () {
        //选择安装路径
        var input_information = external.MainWnd.ReInstall();//默认路径信息
        external.MainWnd.setSpaceCallBackHandler("GetSpace");
        $('#installPath').html(input_information);
    },
    bindEvent: function () {
        //关闭
        $('i.close').bind('click', function () {
            //external.MainWnd.close();
        });

        //点击自定义选项
        $('#btnShowOptions span').toggle(function () {
            external.MainWnd.setSize(520,565);
            //$(this).children('img:eq(0)').hide();
            //$(this).children('img:eq(1)').show();
            $('.section').removeClass('close').addClass('open');
            $('.option-wrap').slideDown(50);
        }, function () {
            //$(this).children('img:eq(0)').show();
             //$(this).children('img:eq(1)').hide();
            $('.section').addClass('close');
            $('.option-wrap').slideUp(50);
            external.MainWnd.setSize(520, 420);
        });

        //点击复选框
        $('.checkbox[id!="btnAgreeService"]').toggle(function () {
            $(this).removeClass('checked');
            $(this).children('input').val('0');
        }, function () {
            $(this).addClass('checked');
            $(this).children('input').val('1');
        });

        //点击同意服务条款
        $('#btnAgreeService').bind('click', function () {
            if ($(this).hasClass('checked')) {
                $(this).removeClass('checked');
                $('#btnStartSetup').addClass('disabled');
            } else {
                $(this).addClass('checked');
                $('#btnStartSetup').removeClass('disabled');
            }
        });

        //点击开始安装
        $('#btnStartSetup').unbind().bind('click', function () {
            if ($(this).hasClass('disabled')) {
                return;
            }

            install($(this));//开始安装
        });

        //取消安装
        $('.btn-setup-cancel').bind('click', function () {
            external.MainWnd.close();
        });

        //无法安装时点击完成
        $('.btn-setup-ok').bind('click', function () {
            //external.MainWnd.close();
            $('.dialogMain').hide();
            $('.dialog').hide();
        });

        //覆盖直接安装
        $('.btn-setup-cover').bind('click', function () {
            if (external.MainWnd.CheckAppRunning())
            {
                handleDialog('force', '应用程序仍在运行，请确认是否强制关闭并重新安装？');
            } else {
                installForce();
            }
        });

        //强制覆盖
        $('.btn-setup-force').bind('click', function () {
            installForce();
        });

        //继续安装
        $('.btn-setup-resetup').bind('click', function () {
            $('.step2').hide();
            $('.success').show();
        });

        //完成安装
        $('.btn-setup-success').bind('click', function () {
            external.MainWnd.close(1);//1：拉起程序  0、空  关闭程序
        });

        //安装失败
        $('.btn-setup-error').bind('click', function () {
            external.MainWnd.close();
        });

        //点击选择安装路径
        $('#btnChoosePath').unbind().bind('click', function () {
            if ($(this).hasClass('disabled')) {
                return;
            }

            var rel=external.MainWnd.selectFolderDialog("请选择安装路径");
            if (rel=='') {
                handleDialog('noRight', '你没有权限把“蜜蜂”安装到该路径');
                return false;
            } else {
                $('#installPath').html(rel);
                external.MainWnd.setSpaceCallBackHandler("GetSpace");
            }
        });

        $("body").keydown(function(e) {
            if (e.ctrlKey == 1) {
                if (e.keyCode==65)
                {
                    return false;
                }
            }
        })
    }
};

//安装空间判断，暂留
function GetSpace(free, need) {
    //$('.needspace').text("所需空间：" + need + "M");
    if(free > 1000) {
        var value = free/1000;
        value = value.toFixed(1);
        //$('.remainspace').text("可用空间：" + value + "G");
    } else {
        //$('.remainspace').text("可用空间：" + value + "M");
    }

    if (free - need < 100) {
        handleDialog('nospace', '您的磁盘空间不足，请清理磁盘或者重新选择安装路径。');
    }
}

//安装状态检查
function statusBar(bValue, nValue) {
    nValue = parseInt(nValue*need_install_meeting);
    //$('.bar').css('backgroundImage', 'linear-gradient(to right, #FFCC00 0%, #FFCC00 '+ nValue +', #ccc '+ nValue +', #ccc 100%)');
    $('.num').css('width', nValue + '%');
    $('.des>.right').text(nValue + '%');
    //$(".load_welcome").text(""+nValue+"%").css("margin-left", ""+nValue-2+"%");
    if (g_bNeedInstallMeeting) {
        //蜜蜂已经装完，开始装云会议。
        if (nValue >= 75 ) {
			if(bValue){
				g_bNeedInstallMeeting = 0;
                need_install_meeting = 1;
                //$('#upload_loadding').show();
                //$("#first").find(".create_two").text("正在安装必要的插件...");
                $('.des>.left').text('正在安装必要的插件...');

                var bRet=external.MainWnd.InstallUCCMeeting();
                if (bRet != 1) {
                    //调起云会议安装包失败。
                    external.MainWnd.setSize(520,420);
                    $(".step2").hide();
                    $('.error2').show();
					$('.dialogMain').hide();
					$('.close').removeClass('disabled');

                }
			} else {
				//安装蜜蜂失败了
				external.MainWnd.setSize(520,420);
				$(".step2").hide();
				$('.error1').show();
				$('.dialogMain').hide();
				$('.close').removeClass('disabled');
			}
			

        }
    }
    
    if ( nValue == 101) {
        external.MainWnd.setSize(520,420);
        $(".step2").hide();
        $('.error1').show();
        $('.dialogMain').hide();
    }

    if (nValue == 100) {
        if ( bValue ) {//安装完成
            //external.MainWnd.setSize(533,373);
            $('.step2').hide();
            $('.success').show();
        } else {
            $('.step2').hide();
            $('.error2').show();
        }

        clearTimeout(load_picture);
        $('.close').removeClass('disabled');
        $('.dialogMain').hide();
    }
}

//开始安装
function install (t) {
    var _bDeskLink = $('input[name="createShortcut"]').val();//生成桌面快捷方式
    var _bLauncher = $('input[name="inLauncher"]').val();//添加到快捷启动栏
    var _bAutoRun = $('input[name="autoRun"]').val();//开机自动启动
    $('.section').removeClass('open');
    $('.step1').hide();
    external.MainWnd.setSize(520, 420);
    $('i.close').addClass('disabled');
    $('.step2').show();
    $('.dialogMain').css({'z-index': 3, 'width': '460px'}).show();
    g_bNeedInstallMeeting = 1;//external.MainWnd.CheckNeedInstallMeeting();
    if (g_bNeedInstallMeeting) {
        need_install_meeting = 0.75;
    }else{
        need_install_meeting = 1;
    }
    external.MainWnd.DoInstall(_bDeskLink, _bLauncher, _bAutoRun);
    scrollBanner();
    external.MainWnd.setCallBackHandler("statusBar");
}

//强制安装
function installForce() {
    external.MainWnd.setSize(1, 1);
    $('.dialogMain').css({'z-index': 3, 'width': '460px'}).show();
    $('.dialog').hide();
    $('.step0').hide();
    $('i.close').addClass('disabled');
    $('.step1').hide();
    $('.section').fadeIn(300, function () {
        $('.step2').show();
        var width = (screen.width-520)/2;
        var height = (screen.height-420)/2
        external.MainWnd.setPos(width, height);
        external.MainWnd.setSize(520, 420);
        external.MainWnd.setWinTitleRect(0,0,480,50);
    });
    g_bNeedInstallMeeting = external.MainWnd.CheckNeedInstallMeeting();
    if (g_bNeedInstallMeeting) {
        need_install_meeting = 0.75;
    } else {
        need_install_meeting = 1;
    }
    external.MainWnd.RecoverInstall();
    external.MainWnd.setCallBackHandler("statusBar");
    scrollBanner();
    //external.MainWnd.setSize(530,370)
}

//安装过程
var index = 1, load_picture = '', isFirst = true;
function scrollBanner() {
    index++;
    if (index > 5) {
        index = 1;
    }

    if (isFirst) {
        isFirst = false;
    } else {
        $('#productImg').fadeOut(function () {
            $(this).attr('src', index + '.png');
            $(this).fadeIn();
        });
    }

	load_picture = setTimeout(function(){scrollBanner()}, 5000);
}

//点击提示框做对应操作
function handleDialog(type, msg) {
    var _content = $('.dialog').find('.content');
    var _options = $('.dialog').find('.btn-option');
    _content.html(msg);
    switch (type) {
        case 'cover':
            _options.find('a').hide();
            _options.find('a:eq(0), a:eq(2)').show();
            break;
        case 'installed':
            _options.find('a').hide();
            _options.find('a:eq(3)').show();
            break;
        case 'force':
            _options.find('a').hide();
            _options.find('a:eq(0), a:eq(1)').show();
            break;
        case 'nospace':
        _options.find('a').hide();
        _options.find('a:eq(3)').show();
            break;
        case 'noRight':
            _options.find('a').hide();
            _options.find('a:eq(3)').show();
            break;
        default:
            break;

    }

    $('.dialogMain').show();
    $('.dialog').show();
    return;

    $('.section').fadeOut(200, function () {
        external.MainWnd.setSize(360,170);
        var width = (screen.width-360)/2;
        var height = (screen.height-170)/2
        external.MainWnd.setPos(width, height);
    	//external.MainWnd.setTitle("小蜜蜂");
    	external.MainWnd.setWinTitleRect(0,0,360,30);
        $('.dialog').show();
    });
}

function change_page()
{
   $('.step0').hide();
   $('.step1').show();
}

//page init
$(document).ready(function () {
    setup.init();//启动页面
});