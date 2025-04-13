import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { BotsLaunchPage } from './pages/bots-launch-page/bots-launch-page.component';

const routes: Routes = [
  { path: 'launch', component: BotsLaunchPage },
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class BotsRoutingModule {}