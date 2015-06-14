/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cookieslawdisclaimer.h"
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WDialog>
#include <Wt/WMessageBox>

using namespace Wt;
using namespace WtCommons;

#define COOKIES_LAW_COOKIE_NAME "cookies_law_disclaimer"
#define COOKIES_LAW_COOKIE_VALUE "agreed"

void CookiesLawDisclaimer::checkOrCreate(WContainerWidget* container, const std::function<void()>& runOnAccepted)
{
  try {
    auto disclaimerAgreed = wApp->environment().getCookie(COOKIES_LAW_COOKIE_NAME);
    if(disclaimerAgreed == "agreed")
      runOnAccepted();
      return;
  } catch(std::exception &) {
  }
  (new CookiesLawDisclaimer(container, runOnAccepted))->show();
}


CookiesLawDisclaimer::~CookiesLawDisclaimer()
{
}

CookiesLawDisclaimer::CookiesLawDisclaimer(WContainerWidget* parent, const std::function<void()> &runOnAccepted)
    : WCompositeWidget(parent)
{
  WContainerWidget *content = new WContainerWidget;
  content->addStyleClass("alert alert-danger");
  content->addWidget(new WText(WString::tr("cookies_law_disclaimer")));
  auto readMoreButton = new WPushButton(WString::tr("cookies_law_readmore"), content);
  readMoreButton->setStyleClass("btn-link");
  readMoreButton->clicked().connect([=](const WMouseEvent &){
    WMessageBox::show(WString::tr("cookies_law_readmore_caption"), WString::tr("cookies_law_readmore_text"), Wt::Ok);
  });
  auto agreeButton = new WPushButton(WString::tr("cookies_law_agree"), content);
  agreeButton->setStyleClass("btn-link");
  agreeButton->clicked().connect([=](const WMouseEvent&){
    wApp->setCookie(COOKIES_LAW_COOKIE_NAME, COOKIES_LAW_COOKIE_VALUE, 60 * 60 * 24 * 365);
    runOnAccepted();
    delete this;
  });
  content->addStyleClass("cookies_law_agreement");
  setImplementation(content);
}
