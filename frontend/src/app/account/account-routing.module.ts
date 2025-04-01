import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { AssetsPage } from './pages/assets/assets-page.component';

const routes: Routes = [
  { path: 'assets', component: AssetsPage },
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class AccountRoutingModule {}