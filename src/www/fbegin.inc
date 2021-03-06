<?php

/*
    Copyright (C) 2014-2015 Deciso B.V.
    Copyright (C) 2012 Jim Pingle
    Copyright (C) 2007-2008 Scott Ullrich <sullrich@gmail.com>
    Copyright (C) 2005-2006 Colin Smith <ethethlay@gmail.com>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
    OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

/* Determine automated help URL. Should output the page name and
   parameters separately */
$uri_split = "";
preg_match("/\/(.*)\?(.*)/", $_SERVER["REQUEST_URI"], $uri_split);

/* If there was no match, there were no parameters, just grab the filename
   Otherwise, use the matched filename from above. */
if (empty($uri_split[0])) {
    $pagename = ltrim($_SERVER["REQUEST_URI"], '/');
} else {
    $pagename = $uri_split[1];
}
/* If the page name is still empty, the user must have requested / (index.php) */
if (empty($pagename)) {
    $pagename = "index.php";
}

/* If the filename is wizard.php, reparse looking for the .xml filename */
if ($pagename == 'wizard.php') {
    $param_split = explode('&', $uri_split[2]);
    foreach ($param_split as $param) {
        if (substr($param, 0, 4) == "xml=") {
            $xmlfile = explode('=', $param);
            $pagename = $xmlfile[1];
        }
    }
}

$aclObj = new \OPNsense\Core\ACL();

/* display a top alert bar if need be */
$need_alert_display = false;
$found_notices = are_notices_pending();
if($found_notices == true) {
    $notices = get_notices();
    if(!$notices) {
        $need_alert_display = true;
        $display_text = print_notices($notices) . "<br />";
    }
}
if($need_alert_display == true) {
    echo "<div class=\"col-xs-12\"><div class=\"alert alert-info\"><p>".$display_text."</p></div></div>";
}

?>

<header class="page-head">
  <nav class="navbar navbar-default" role="navigation">
    <div class="container-fluid">
      <div class="navbar-header">
        <a class="navbar-brand" href="/index.php">
        <img class="brand-logo" src="/ui/themes/<?= $themename ?>/build/images/default-logo.png" height="30" alt="logo" />
        <img class="brand-icon" src="/ui/themes/<?= $themename ?>/build/images/icon-logo.png" height="30" alt="icon" />
        </a>
        <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navigation">
          <span class="sr-only">Toggle navigation</span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
        </button>
      </div>
      <div class="collapse navbar-collapse">
        <ul class="nav navbar-nav navbar-right">
          <li id="menu_messages"><?= get_menu_messages() ?></li>
          <li>
            <form class="navbar-form" role="search">
              <div class="input-group">
                <div class="input-group-addon"><i class="fa fa-search"></i></div>
                <input type="text" style="width: 250px;" class="form-control" tabindex="1" data-provide="typeahead" id="menu_search_box">
              </div>
            </form>
          </li>
        </ul>
      </div>
    </div>
  </nav>
</header>

<main class="page-content col-sm-10 col-sm-push-2">
    <aside id="navigation" class="page-side col-xs-12 col-sm-2 hidden-xs">
        <div class="row">
            <nav class="page-side-nav" role="navigation">
                <div id="mainmenu" class="panel" style="border:0px" >
                    <div class="panel list-group" style="border:0px">
<?php
                          foreach($menuSystem as $topMenuItem): ?>
                            <a href="#<?=$topMenuItem->Id;?>" class="list-group-item <?= $topMenuItem->Selected ? 'active-menu-title' : ""; ?>" data-toggle="collapse" data-parent="#mainmenu"><span class="<?=$topMenuItem->CssClass;?> __iconspacer"></span><?=gettext($topMenuItem->VisibleName);?>  </a>
                            <div class="collapse <?=$topMenuItem->Selected ? 'active-menu in' :"";?>" id="<?=$topMenuItem->Id;?>">
<?php
                            foreach($topMenuItem->Children as $subMenuItem): ?>
<?php
                              if ($subMenuItem->Url == '' ):?>
                               <a href="#<?=$topMenuItem->Id;?>_<?=$subMenuItem->Id;?>" class="list-group-item <?=$subMenuItem->Selected ? "active-menu-title" : "";?>" data-toggle="collapse" data-parent="#<?=$topMenuItem->Id;?>" aria-expanded="<?=$subMenuItem->Selected ? "true" : "false";?>">
                                  <div style="display: table;width: 100%;">
                                    <div style="display: table-row">
                                      <div style="display: table-cell"><?=gettext($subMenuItem->VisibleName);?></div>
                                        <div style="display: table-cell; text-align:right; vertical-align:middle;"><span class="<?=$subMenuItem->CssClass;?>"></span></div>
                                      </div>
                                  </div>
                               </a>
                               <div class="collapse <?=$subMenuItem->Selected ? "active-menu in" :"";?>" id="<?=$topMenuItem->Id;?>_<?=$subMenuItem->Id;?>">
<?php
                                  foreach ($subMenuItem->Children as $subsubMenuItem):?>
<?php
                                    if ($subsubMenuItem->IsExternal == "Y"):?>
                                            <a href="<?=$subsubMenuItem->Url;?>" target="_blank" class="list-group-item menu-level-3-item <?=$subsubMenuItem->Selected ? "active" :"";?>"><?=gettext($subsubMenuItem->VisibleName);?></a>
<?php
                                    elseif ($aclObj->isPageAccessible($_SESSION['Username'],$subsubMenuItem->Url)):?>
                                            <a href="<?=$subsubMenuItem->Url;?>" class="list-group-item menu-level-3-item <?=$subsubMenuItem->Selected ? "active" :"";?>"><?=gettext($subsubMenuItem->VisibleName);?></a>
<?php
                                    endif;
                                  endforeach;?>
                                </div>
<?php
                              elseif ($subMenuItem->IsExternal == "Y" ):?>
                                <a href="<?=$subMenuItem->Url;?>" target="_blank" class="list-group-item <?=$subMenuItem->Selected ? "active" : "";?>" aria-expanded="<?=$subMenuItem->Selected ? "true" : "false";?>">
                                  <div style="display: table;width: 100%;">
                                    <div style="display: table-row">
                                      <div style="display: table-cell"><?=gettext($subMenuItem->VisibleName);?></div>
                                        <div style="display: table-cell; text-align:right; vertical-align:middle;"><span class="<?=$subMenuItem->CssClass;?>"></span></div>
                                      </div>
                                  </div>
                                </a>
<?php
                              elseif ($aclObj->isPageAccessible($_SESSION['Username'],$subMenuItem->Url)):?>
                                <a href="<?=$subMenuItem->Url;?>" class="list-group-item <?=$subMenuItem->Selected ? "active" :"";?>">
                                  <div style="display: table;width: 100%;">
                                    <div style="display: table-row">
                                      <div style="display: table-cell"><?=gettext($subMenuItem->VisibleName);?></div>
                                        <div style="display: table-cell; text-align:right; vertical-align:middle;"><span class="<?=$subMenuItem->CssClass;?>"></span></div>
                                    </div>
                                  </div>
                                </a>
<?php
                              endif;?>
<?php
                            endforeach; ?>
                            </div>
<?php
                        endforeach; ?>
                    </div>
                </div>
            </nav>
        </div>
    </aside>
  <div class="row">
    <header class="page-content-head">
      <div class="container-fluid">
          <form action="<?=$_SERVER['REQUEST_URI'];?>" method="post">
        <ul class="list-inline">
          <li class="__mb"><h1><?= gentitle($menuBreadcrumbs) ?></h1></li>
          <li class="btn-group-container">
            <?php
            if (isset($service_hook)) {
              $ssvc = find_service_by_name($service_hook);
              if (!empty($ssvc)) {
                echo get_service_status_icon($ssvc, false);
                echo get_service_control_links($ssvc, false);
              }
            }
            ?>
            <?php if (!empty($main_buttons)): foreach ($main_buttons as $button): ?>
              <a href="<?=$button['href'];?>" class="btn btn-primary"><span class="glyphicon glyphicon-plus-sign __iconspacer"></span><?=$button['label'];?></a>
            <?php endforeach; endif; ?>

            <?php if (isset($widgetfiles)): ?>
              <a href="#" id="updatepref" style="display:none" onclick="return updatePref();" class="btn btn-primary"><?=gettext("Save Settings");?></a>
              <button type="button" class="btn btn-default" data-toggle="modal" data-target="#modal_widgets"><span class="glyphicon glyphicon-plus-sign __iconspacer"></span><?= gettext('Add widget') ?></button>
            <?php endif; ?>
          </li>
        </ul>
          </form>
      </div>
    </header>
